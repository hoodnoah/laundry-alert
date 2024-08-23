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

const buffer_size_readings = 520
const max_flush_time_seconds = time.Second * 10

type AccelerometerReading struct {
	TimeStampMs uint32 `json:"timeStampMs"`
	ReceivedAt  time.Time
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
	readingBuffer   []AccelerometerReading
	lastFlush       time.Time
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

	readingsChannel := make(chan *AccelerometerReading, 5*104)
	readingsBuffer := make([]AccelerometerReading, 0, buffer_size_readings)

	connection := SqLiteDbConnection{
		db:              db,
		readingsChannel: readingsChannel,
		logger:          logger,
		readingBuffer:   readingsBuffer,
		lastFlush:       time.Now(),
	}

	go connection.startWriting()

	return &connection, nil
}

// start writing to the database
func (s *SqLiteDbConnection) startWriting() {
	// set up ticker for expiration of buffer
	bufferExpired := time.NewTicker(max_flush_time_seconds)
	defer bufferExpired.Stop()

	for {
		select {
		case reading, ok := <-s.readingsChannel:
			// if channel is closed, write everything and exit
			if !ok {
				// If the channel is closed, flush remaining buffer and exit
				if len(s.readingBuffer) > 0 {
					if err := s.flushBuffer(); err != nil {
						s.logger.Error(err.Error())
					}
				}
				return
			}

			// Otherwise add new reading to the buffer
			s.readingBuffer = append(s.readingBuffer, *reading)

			// Flush a full buffer
			if len(s.readingBuffer) >= buffer_size_readings {
				if err := s.flushBuffer(); err != nil {
					s.logger.Error(err.Error())
				}
			}

		// Periodically check if the buffer needs flushing due to inactivity
		case <-bufferExpired.C:
			if len(s.readingBuffer) > 0 {
				if err := s.flushBuffer(); err != nil {
					s.logger.Error(err.Error())
				}
			}
		}
	}
}

func (s *SqLiteDbConnection) clearBuffer() {
	s.readingBuffer = s.readingBuffer[:0] // wipe buffer
	s.lastFlush = time.Now()              // reset timer
}

// flush multiple readings to the database as a transaction
func (s *SqLiteDbConnection) flushBuffer() error {
	// always clear the buffer
	defer s.clearBuffer()

	// begin transaction
	tx, err := s.db.Begin()
	if err != nil {
		return err
	}

	stmt, err := tx.Prepare("INSERT INTO accelerometer_readings(timestamp_ms, received_at, x, y, z) VALUES (?, ?, ?, ?, ?)")
	if err != nil {
		return err
	}
	defer stmt.Close()

	for _, element := range s.readingBuffer {
		_, err = stmt.Exec(element.TimeStampMs, element.ReceivedAt, element.X, element.Y, element.Z)
		if err != nil {
			tx.Rollback()
			return err
		}
	}

	err = tx.Commit()
	if err != nil {
		return err
	}

	return nil
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
// func (s SqLiteDbConnection) write(reading *AccelerometerReading) error {
// 	currentTime := time.Now()
// 	stmt, err := s.db.Prepare("INSERT INTO accelerometer_readings(timestamp_ms, received_at, x, y, z) VALUES (?, ?, ?, ?, ?)")

// 	if err != nil {
// 		return err
// 	}
// 	defer stmt.Close()

// 	_, err = stmt.Exec(reading.TimeStampMs, currentTime, reading.X, reading.Y, reading.Z)
// 	if err != nil {
// 		return err
// 	}

// 	return nil
// }
