package database

import (
	// std
	"database/sql"
	"fmt"
	"time"

	// external
	_ "github.com/mattn/go-sqlite3"

	// local
	"github.com/hoodnoah/laundry-alert/logging_backend/logging"
)

const createTableString = `
	CREATE TABLE IF NOT EXISTS accelerometer_readings (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	timestamp_ms INTEGER,
	received_at DATETIME,
	x REAL,
	y REAL,
	z REAL
);`

type AccelerometerReading struct {
	TimeStampMs uint32  `json:"timeStampMs"`
	X           float32 `json:"x"`
	Y           float32 `json:"y"`
	Z           float32 `json:"z"`
}

type IDBConnection interface {
	AddReading(reading *AccelerometerReading)
}

type SqLiteDbConnection struct {
	readingsChannel chan *AccelerometerReading
	db              *sql.DB
	logger          logging.ILogger
}

// constructor for an sqlite database connection
func NewSqLiteDbConnection(connectionString string, logger logging.ILogger) (IDBConnection, error) {
	// create sqlite connection
	db, err := sql.Open("sqlite3", connectionString)
	if err != nil {
		return nil, fmt.Errorf("failed to open SQLite database with error: %v", err)
	}

	// bootstrap sqlite table
	err = bootStrapTable(db)
	if err != nil {
		return nil, err
	}

	readingsChannel := make(chan *AccelerometerReading, 5)

	connection := SqLiteDbConnection{
		db:              db,
		readingsChannel: readingsChannel,
		logger:          logger,
	}

	go connection.startWriting()

	return &connection, nil
}

func (s *SqLiteDbConnection) startWriting() {
	for reading := range s.readingsChannel {
		err := s.write(reading)
		if err != nil {
			s.logger.Error(err.Error())
		}
	}
}

func bootStrapTable(db *sql.DB) error {
	_, err := db.Exec(createTableString)
	if err != nil {
		return fmt.Errorf("failed to create table: %v", err)
	}

	return nil
}

// adds a reading to the channel, to be written by a separate goroutine to the db
func (s *SqLiteDbConnection) AddReading(reading *AccelerometerReading) {
	s.readingsChannel <- reading
}

// writes readings to the database from the channel
func (s SqLiteDbConnection) write(reading *AccelerometerReading) error {
	currentTime := time.Now()
	stmt, err := s.db.Prepare("INSERT INTO accelerometer_readings(timestamp_ms, received_at, x, y, z) VALUES (?, ?, ?, ?, ?)")

	if err != nil {
		return err
	}
	defer stmt.Close()

	_, err = stmt.Exec(reading.TimeStampMs, currentTime, reading.X, reading.Y, reading.Z)
	if err != nil {
		return err
	}

	return nil
}
