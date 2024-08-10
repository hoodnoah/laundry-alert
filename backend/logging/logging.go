package logging

import (
	"fmt"
	"time"
)

type LogLevel int

const (
	Info LogLevel = iota
	Warning
	Error
)

type logMessage struct {
	timeStamp time.Time
	level     LogLevel
	message   string
}

func (m logMessage) String() string {
	var levelText string

	// convert level enum to string
	switch level := m.level; level {
	case Info:
		levelText = "INFO"
	case Warning:
		levelText = "WARNING"
	case Error:
		levelText = "ERROR"
	}

	// convert time to time string
	timeStampString := m.timeStamp.Format("2006-01-02 03:04:05 PM")

	return fmt.Sprintf("[%s] %s - %s", levelText, timeStampString, m.message)
}

type ILogger interface {
	Info(message string)
	Warning(message string)
	Error(message string)
}

type ConsoleLogger struct {
	messages chan logMessage
}

// creates a console logger
// and starts printing to the console as messages are posted
func ConsoleLoggerNew() ILogger {
	messagesChannel := make(chan logMessage, 5)
	instance := &ConsoleLogger{
		messages: messagesChannel,
	}

	go instance.writeToConsole()

	return instance
}

func prepMessage(level LogLevel, message string) logMessage {
	return logMessage{
		timeStamp: time.Now(),
		level:     level,
		message:   message,
	}
}

// local function for writing a message to console
func (l ConsoleLogger) writeToConsole() {
	for msg := range l.messages {
		fmt.Println(msg.String())
	}
}

func (l *ConsoleLogger) Info(message string) {
	l.messages <- prepMessage(Info, message)
}

func (l *ConsoleLogger) Warning(message string) {
	l.messages <- prepMessage(Warning, message)
}

func (l *ConsoleLogger) Error(message string) {
	l.messages <- prepMessage(Error, message)
}
