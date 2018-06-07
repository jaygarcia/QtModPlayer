-- DROP TABLE IF EXISTS songs;
DROP TABLE IF EXISTS plays;
DROP INDEX IF EXISTS song_index;
DROP TABLE IF EXISTS eqSettings;
DROP TABLE IF EXISTS playlists;
DROP TABLE IF EXISTS playlist_prototype;
DROP TABLE IF EXISTS playlist_songs_tmp;

-- CREATE TABLE songs (
--     id_md5     TEXT,
--     song_name  TEXT,
--     name       TEXT, 
--     file_name_short  TEXT, 
--     directory  TEXT, 
--     like_value INT,
--     in_queue   INT
-- );


CREATE TABLE playlists (
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    dated_modified timestamp default (strftime('%s', 'now')),
    playlist_name TEXT,
    playlist_table_name TEXT
);


CREATE TABLE playlist_prototype (
    song_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    song_name  TEXT,
    file_name  TEXT, 
    full_path  TEXT, 
    like_value INT,
    in_queue   INT default(0),
    unique(full_path)
);

-- CREATE 
--     INDEX 
--         song_index 
--     ON 
--         playlist_songs(song_name, file_name) 
--     WHERE 
--         song_name 
--     IS 
--         NOT NULL; 


/*
    centerFrequencies[0] = 32.0f;
    centerFrequencies[1] = 64.0f;
    centerFrequencies[2] = 125.0f;
    centerFrequencies[3] = 250.0f;
    centerFrequencies[4] = 500.0f;
    centerFrequencies[5] = 1000.0f;
    centerFrequencies[6] = 2000.0f;
    centerFrequencies[7] = 4000.0f;
    centerFrequencies[8] = 8000.0f;
    centerFrequencies[9] = 16000.0f;
*/
CREATE TABLE eqSettings (
    id_md5      TEXT,
    name        TEXT,
    freq32Hz    TEXT,
    freq64Hz    TEXT,
    freq125Hz   TEXT,
    freq250Hz   TEXT,
    freq500Hz   TEXT,
    freq1kHz    TEXT,
    freq2kHz    TEXT,
    freq4kHz    TEXT,
    freq8kHz    TEXT,
    freq16kHz   TEXT
);


insert into eqSettings VALUES (
    "",
    "Flat",     -- Name 
    "0",     --32
    "0",     --64
    "0",     --125
    "0",     --250
    "0",     --500
    "0",     --1K
    "0",     --2K
    "0",     --4K
    "0",     --8K
    "0"      --16K
);

insert into eqSettings VALUES (
    "",
    "Acoustic",     -- Name 
    "3",     --32
    "3",     --64
    "2.5",     --125
    ".75",     --250
    "1.25",     --500
    "1.25",     --1K
    "2.25",     --2K
    "2.75",     --4K
    "2.5",     --8K
    "1.25"      --16K
);

insert into eqSettings VALUES (
    "",
    "Bass Booster",     -- Name 
    "3.5",     --32
    "2.5",     --64
    "2.25",     --125
    "1.75",     --250
    ".75",     --500
    "0",     --1K
    "0",     --2K
    "0",     --4K
    "0",     --8K
    "0"      --16K
);

insert into eqSettings VALUES (
    "",
    "Bass Reducer",     -- Name 
    "-3.5",     --32
    "-2.5",     --64
    "-2.25",     --125
    "-1.75",     --250
    "-.75",     --500
    "0",     --1K
    "0",     --2K
    "0",     --4K
    "0",     --8K
    "0"      --16K
);

insert into eqSettings VALUES (
    "",
    "Classical",     -- Name 
    "3",     --32
    "2.25",     --64
    "1.9",     --125
    "1.75",     --250
    "-1",     --500
    "-1",     --1K
    "0",     --2K
    "1.5",     --4K
    "2.25",     --8K
    "2.5"      --16K
);



insert into eqSettings VALUES (
    "",
    "Dance",     -- Name 
    "2.5",     --32
    "4.5",     --64
    "3.5",     --125
    "0",     --250
    "1",     --500
    "2.5",     --1K
    "3.5",     --2K
    "3",     --4K
    "2.5",     --8K
    "0"      --16K
);


insert into eqSettings VALUES (
    "",
    "Deep",     -- Name 
    "3",     --32
    "2.25",     --64
    "1",     --125
    ".5",     --250
    "1.95",     --500
    "1.75",     --1K
    ".95",     --2K
    "1.15",     --4K
    "-2.5",     --8K
    "-3.25"      --16K
);

insert into eqSettings VALUES (
    "",
    "Electronic",     -- Name 
    "2.75",     --32
    "2.5",     --64
    ".75",     --125
    "0",     --250
    "-1.25",     --500
    "1.75",     --1K
    ".75",     --2K
    ".95",     --4K
    "2.75",     --8K
    "3.25"      --16K
);


insert into eqSettings VALUES (
    "",
    "Hip-Hop",     -- Name 
    "3.25",     --32
    "2.75",     --64
    ".95",     --125
    "2",     --250
    "-.95",     --500
    "-.95",     --1K
    ".95",     --2K
    "-.25",     --4K
    "1.25",     --8K
    "1.95"      --16K
);

insert into eqSettings VALUES (
    "",
    "Jazz",     -- Name 
    "2.75",     --32
    "1.95",     --64
    ".95",     --125
    "1.5",     --250
    "-1",     --500
    "-1",     --1K
    "0",     --2K
    ".95",     --4K
    "1.98",     --8K
    "2.5"      --16K
);

insert into eqSettings VALUES (
    "",
    "Loudness",     -- Name 
    "3.95",     --32
    "2.95",     --64
    "0",     --125
    "0",     --250
    "-1.75",     --500
    "0",     --1K
    "-.95",     --2K
    "3.5",     --4K
    "3.5",     --8K
    ".75"      --16K
);

insert into eqSettings VALUES (
    "",
    "Pop",     -- Name 
    "-1",     --32
    "-.75",     --64
    "0",     --125
    "1.5",     --250
    "2.25",     --500
    "2.25",     --1K
    "-1.75",     --2K
    "0",     --4K
    "-.5",     --8K
    "-1"      --16K
);

insert into eqSettings VALUES (
    "",
    "R&B",     -- Name 
    "1.95",     --32
    "4.75",     --64
    "3.75",     --125
    ".95",     --250
    "-1.25",     --500
    "-1",     --1K
    "1.75",     --2K
    "1.95",     --4K
    "2",     --8K
    "2.25"      --16K
);

insert into eqSettings VALUES (
    "",
    "Rock",     -- Name 
    "3.25",     --32
    "2.5",     --64
    "2",     --125
    "1",     --250
    "-.5",     --500
    "-.75",     --1K
    ".25",     --2K
    "1.95",     --4K
    "2.5",     --8K
    "3"      --16K
);



insert into eqSettings VALUES (
    "",
    "Trebel Booster",     -- Name 
    "0",     --32
    "0",     --64
    "0",     --125
    "0",     --250
    "0",     --500
    ".75",     --1K
    "1.55",     --2K
    "2.25",     --4K
    "2.95",     --8K
    "3.75"      --16K
);



insert into eqSettings VALUES (
    "",
    "Trebel Reducer",     -- Name 
    "0",     --32
    "0",     --64
    "0",     --125
    "0",     --250
    "0",     --500
    "-.75",     --1K
    "-1.55",     --2K
    "-2.25",     --4K
    "-2.95",     --8K
    "-3.75"      --16K
);


insert into eqSettings VALUES (
    "",
    "Vocal Booster",     -- Name 
    "-1",     --32
    "-2",     --64
    "-2",     --125
    ".95",     --250
    "2.25",     --500
    "2.25",     --1K
    "2",     --2K
    "1",     --4K
    "0",     --8K
    "-1"      --16K
);

vacuum;
