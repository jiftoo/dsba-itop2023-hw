use std::{collections::HashSet, panic::catch_unwind, hash::Hash};

fn main() {
	let file = std::fs::File::open("imdb.csv").unwrap();
	let mut reader = csv::Reader::from_reader(file);

	#[derive(Debug)]
	struct Film {
		name: String,
		date: u32,  // year
		rate: f32,  // rating
		votes: u32, // voters
		genre: Vec<String>,
		duration: u32, // minutes
		_type: String,
		certificate: String,
		nudity: String,
		violence: String,
	}

	let mut dedup = HashSet::new();
	let mut films: Vec<Film> = vec![];

	let mut read = 0;
	let mut read_not_dupe = 0;

	for film_entry in reader.records().filter_map(|x| x.ok()) {
		let vec_ptr = &mut films as *mut Vec<Film>;
		let set_ptr = &mut dedup as *mut HashSet<String>;
		let r1 = &mut read as *mut i32;
		let r2 = &mut read_not_dupe as *mut i32;
		catch_unwind(|| {
			let mut film = Film {
				name: film_entry[0].to_string(),
				date: film_entry[1].parse::<u32>().unwrap(),
				rate: film_entry[2].parse::<f32>().unwrap_or(0.0),
				votes: film_entry[3].replace(',', "").parse::<u32>().unwrap_or(0),
				genre: film_entry[4].split(',').map(|x| x.trim().to_string()).collect(),
				duration: film_entry[5].trim().parse::<u32>().unwrap_or(0),
				_type: film_entry[6].to_string().to_lowercase(),
				certificate: film_entry[7].to_string().to_uppercase(),
				nudity: film_entry[9].to_string().to_lowercase(),
				violence: film_entry[10].to_string().to_lowercase(),
			};

			unsafe {
				*r1 += 1;
				if !set_ptr.as_mut().unwrap().insert(film.name.clone()) {
					return;
				}
				*r2 += 1;
			}

			film.certificate = match film.certificate.as_str() {
				x
				@ ("TV-Y" | "TV-Y7" | "G" | "TV-G" | "PG" | "TV-PG" | "PG-13" | "TV-14" | "R" | "TV-MA" | "NC-17") => x.to_string(),
				_ => "unknown".to_string(),
			};

			unsafe {
				vec_ptr.as_mut().unwrap().push(film);
			}
		})
		.map_err(|_| {
			println!("Error: {:?}", film_entry);
			panic!();
		})
		.unwrap();
	}

	let mut writer = csv::Writer::from_path("imdb_well_formed.csv").unwrap();
	writer
		.write_record([
			"name",
			"date",
			"rate",
			"votes",
			"genre",
			"duration",
			"type",
			"certificate",
			"nudity",
			"violence",
		])
		.unwrap();

	println!("{}", films.iter().filter(|x| x.date > 2020).count());
	println!("{:?}", films.iter().filter_map(|x| Some(x.nudity.clone())).collect::<HashSet<_>>());
	println!("{:?}", films.iter().filter_map(|x| Some(x.genre.clone())).flatten().collect::<HashSet<_>>());


	for film in films {
		writer
			.write_record([
				film.name,
				film.date.to_string(),
				film.rate.to_string(),
				film.votes.to_string(),
				film.genre.join(","),
				film.duration.to_string(),
				film._type,
				film.certificate,
				film.nudity,
				film.violence,
			])
			.unwrap();
	}

	println!("Transformed {}, saved {}", read, read_not_dupe);
}
