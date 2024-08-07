package main

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

type State struct {
	Active bool `json:"active"`
}

func main() {
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
			c.Error(err)
			c.AbortWithStatus(http.StatusBadRequest)
			return
		}

		washingMachineState.Active = content.Active
	})

	r.Run()
}
