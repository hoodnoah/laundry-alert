package main

import (
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"

	"github.com/hoodnoah/laundry-alert/backend/logging"
)

type State struct {
	Active bool `json:"active"`
}

func main() {
	logger := logging.ConsoleLoggerNew()

	washingMachineState := State{
		Active: false,
	}

	r := gin.Default()

	// return the current state from a get request
	r.GET("/state", func(c *gin.Context) {
		c.JSON(http.StatusOK, gin.H{
			"active": washingMachineState.Active,
		})
	})

	// set the state from a post request
	r.POST("/state", func(c *gin.Context) {
		var content State
		if err := c.ShouldBindJSON(&content); err != nil {
			logger.Error("Received malformed input, rejecting with Status 400")

			c.Error(err)
			c.AbortWithStatus(http.StatusBadRequest)
			return
		}

		washingMachineState.Active = content.Active
		logger.Info(fmt.Sprintf("Set Active: %t", washingMachineState.Active))

		// confirm receipt
		c.JSON(http.StatusAccepted, gin.H{
			"active": washingMachineState.Active,
		})
	})

	r.Run()
}
