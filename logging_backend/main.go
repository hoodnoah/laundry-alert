package main

import (
	"fmt"
	"net/http"
	"os"
	"time"

	"github.com/gin-gonic/gin"

	"github.com/hoodnoah/laundry-alert/logging_backend/database"
	"github.com/hoodnoah/laundry-alert/logging_backend/logging"
)

func main() {
	// set up logging
	logger := logging.ConsoleLoggerNew()

	// set up db connection
	dbConnection, err := database.NewSqLiteDbConnection("laundry-logger.db", logger)
	if err != nil {
		fmt.Printf("failed to set up db connection w/ error: %v", err)
		os.Exit(1)
	}

	r := gin.Default()

	r.POST("/reading", func(c *gin.Context) {
		var content database.AccelerometerReading
		if err := c.ShouldBindJSON(&content); err != nil {
			c.Error(err)
			c.AbortWithStatus(http.StatusBadRequest)
			return
		}

		// add absolute timestamp
		content.ReceivedAt = time.Now()

		// add to db
		dbConnection.AddReading(&content)

		// c.JSON(http.StatusAccepted, gin.H{"message": "received, written"})
		// send simple confirmation of receipt
		c.String(http.StatusAccepted, "")
	})

	r.Run()
}
