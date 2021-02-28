const http = require("http");

const host = 'localhost';
const port = 8000;
const token = Math.random().toString(36).substr(2) + Math.random().toString(36).substr(2);

const requestListener = function (req, res) {
    console.log(`\n===============\nreq.method: ${req.method}`);
    
    console.log("authorization: " + req.headers["authorization"]);
    console.log("token: " + token);
    console.log("req.url: " + req.url);
    console.log('Headers:', req.headers);
    switch (req.url)
    {
    case "/auth":
        if (req.headers["authorization"] == "test:123")
        {
            res.setHeader("Content-Type", "application/json");
            res.writeHead(200);
            res.end(`{"token": "${token}"}`);
        }
        else
        {
            // res.setHeader("Content-Type", "application/json");
            res.writeHead(401);
            res.end();
        }
        break
    case "/get_pl":
        if (req.headers["authorization"] == token)
        {
            res.setHeader("Content-Type", "application/json");
            res.writeHead(200);
            res.end(JSON.stringify({
              Patients: [
                  {
                    FirstName:  'Dmitry1',
                    SecondName: 'Klinov',
                    Diagnose: 'Acrophobia'
                  },
                  {
                    FirstName:  'Dmitry2',
                    SecondName: 'Klinov',
                    Diagnose: 'Acrophobia'
                  },
                  {
                    FirstName:  'Petr1',
                    SecondName: 'Sazanov',
                    Diagnose: 'Acrophobia'
                  },
                  {
                    FirstName:  'Dmitry3',
                    SecondName: 'Klinov',
                    Diagnose: 'Acrophobia'
                  },
                  {
                    FirstName:  'Petr2',
                    SecondName: 'Sazanov',
                    Diagnose: 'Acrophobia'
                  }
                ]
            }))
        }
        else
        {
            res.writeHead(401);
            res.end();
        }
        break


    default:
        res.writeHead(200);
        res.end(`{"message": "This is a JSON default response"}`);

    }
};

const server = http.createServer(requestListener);
server.listen(port, host, () => {
    console.log(`Server is running on http://${host}:${port}`);
});
