# Qt 6.5.1 Year one project.

it was fun speedrunning this from 0 to 100 in a few days.

# Building

- Install qt 6.5.1
- Make sure Qt Creator is available
- Clone this repo
- Open `/CMakeLists.txt` with Qt Creator
- Build
- ???
- Profit

# Building the rust thing

<div id="building-rust" />

- Download the [Kaggle dataset](https://www.kaggle.com/datasets/mazenramadan/imdb-most-popular-films-and-series)
- Rename the downloaded file to 'imdb.csv'
- Place it in `/specs`
- [Install rust](https://www.rust-lang.org/tools/install)
- Run `cargo build` in `/specs`
- Salvage `imdb_well_formed.csv`

# Running

- Build the Qt app
- Run it
- Navigate to `File -> New...`
- Save the new file in a directory
- Proceed using the app

Optionally, follow the steps in [Building the rust thing](#building-the-rust-thing), then run and navigate to `File -> Open...`, open `imdb_well_formed.csv` and proceed using the app.
