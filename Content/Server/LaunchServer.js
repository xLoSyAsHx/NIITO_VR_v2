const express = require('express'),
  socket = require('socket.io'),
  mysql = require('mysql'),
  cookieParser = require('cookie-parser'),
  session = require('express-session'),
  util = require('util');
var clients = []

//App setup
var app = express();
var server = app.listen(3000, function () {
  console.log("listening to port 3000.");
});
var io = socket(server);








io.on('connection', function(socket){
  clients.push(socket.id);
  var clientConnectedMsg = 'User connected ' + util.inspect(socket.id) + ', total: ' + clients.length;
  console.log(clientConnectedMsg);

  socket.on('login', function(data, ackFn){
    console.log("Recieve 'login' event from "  + util.inspect(socket.id) + " user");
    console.log(JSON.parse(data))
    json_data = JSON.parse(data)
    console.log('Username: ' + json_data.Username + '; Password: ' + json_data.Password)

    ackFn(JSON.stringify({
      CanLogin: true,
      Patients: [
          {
            FirstName:  'Dmitry1',
            SecondName: 'Klinov'
          },
          {
            FirstName:  'Dmitry2',
            SecondName: 'Klinov'
          },
          {
            FirstName:  'Petr1',
            SecondName: 'Sazanov'
          },
          {
            FirstName:  'Dmitry3',
            SecondName: 'Klinov'
          },
          {
            FirstName:  'Petr2',
            SecondName: 'Sazanov'
          }
        ]
    }))
  })

  socket.on('disconnect', function(){
    clients.pop(socket.id);
    var clientDisconnectedMsg = 'User disconnected ' + util.inspect(socket.id) + ', total: ' + clients.length;
    console.log(clientDisconnectedMsg);
  })
});