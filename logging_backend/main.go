package main

import (
	"fmt"
	"net/http"
	"os"
	"time"

	// external
	"github.com/gin-gonic/gin"
	"github.com/gorilla/websocket"

	"github.com/hoodnoah/laundry-alert/logging_backend/database"
	"github.com/hoodnoah/laundry-alert/logging_backend/logging"
)

var upgrader = websocket.Upgrader{
	CheckOrigin: func(r *http.Request) bool {
		return true // allow all origins
	},
}

func main() {
	// set up logging
	logger := logging.ConsoleLoggerNew()

	// set up db connection
	dbConnection, err := database.NewSqLiteDbConnection("/opt/laundry-logger/laundry-logger.db", logger)
	if err != nil {
		fmt.Printf("failed to set up db connection w/ error: %v", err)
		os.Exit(1)
	}

	r := gin.Default()

	r.GET("/ws", func(c *gin.Context) {
		// upgrade HTTP connection to WebSocket
		conn, err := upgrader.Upgrade(c.Writer, c.Request, nil)
		if err != nil {
			logger.Error(fmt.Sprintf("failed to upgrade to websocket: %v", err))
			os.Exit(1)
		}
		defer conn.Close()

		for {
			var content database.AccelerometerReading
			err := conn.ReadJSON(&content)
			if err != nil {
				if websocket.IsUnexpectedCloseError(err) {
					logger.Error(fmt.Sprintf("unexpected close error: %v", err))
				} else {
					logger.Error(fmt.Sprintf("failed to read JSON from connection with error: %v", err))
				}
				break
			}
			// Add absolute timestamp
			content.ReceivedAt = time.Now()

			// Add to DB
			dbConnection.AddReading(&content)

			// Send simple confirmation of receipt
			if err := conn.WriteMessage(websocket.TextMessage, []byte("received")); err != nil {
				logger.Error(fmt.Sprintf("failed to send confirmation: %v", err))
				break
			}

		}

	})

	// r.POST("/reading", func(c *gin.Context) {
	// 	var content database.AccelerometerReading
	// 	if err := c.ShouldBindJSON(&content); err != nil {
	// 		c.Error(err)
	// 		c.AbortWithStatus(http.StatusBadRequest)
	// 		return
	// 	}

	// 	// add absolute timestamp
	// 	content.ReceivedAt = time.Now()

	// 	// add to db
	// 	dbConnection.AddReading(&content)

	// 	// c.JSON(http.StatusAccepted, gin.H{"message": "received, written"})
	// 	// send simple confirmation of receipt
	// 	c.String(http.StatusAccepted, "")
	// })

	r.Run()
}
