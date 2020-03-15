const express = require('express')
const readline = require('readline')

const port = process.env.PORT || 12002
const app = express()

let responses = []
let playerCount = 0
let timer

let winnerResponse
let loseResponses = []

app.get('/connect', (req, res, next) => {
  console.log(`New player from IP ${req.ip}`)
  responses.push(res)
})

app.get('/play', (req, res, next) => {
  playerCount++
  console.log(`Pressed by IP ${req.ip}`)
  if (playerCount === 3)
    winnerResponse = res
  else
    loseResponses.push(res)
})

app.listen(port, () => {
  console.log(`Listening on port ${port}`)
})

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
})

const waitForLine = question => {
  return new Promise((resolve, reject) => {
    rl.question(question, answer => {
      resolve(answer)
    })
  })
}

const delayedTask = (callback, delay) => {
  return new Promise((resolve, reject) => {
    setTimeout(() => {
      callback()
      resolve()
    }, delay)
  })
}

const game = async () => {
  responses = []
  winnerResponse = undefined
  loseResponses = []
  await waitForLine('Press Enter to start game')
  console.log(responses.length)
  for (let response of responses)
    response.sendStatus(200)
  await delayedTask(() => {
    winnerResponse && winnerResponse.status(200).send('won')
    for (let response of loseResponses)
      response.status(200).send('lost')
  }, 60 * 1000)
}

(async () => {
  while (true) {
    await game()
  }
})()
