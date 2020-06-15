const express = require('express')
var exec = require('child_process').exec;
const app = express()
const port = 3000

// app.get('/', (req, res) => res.send('Hello World!'))
app.get('/', (req, res) => {
	exec('cat *.js',
	    function (error, stdout, stderr) {
	        console.log('stdout: ' + stdout);
	        console.log('stderr: ' + stderr);
	        if (error !== null) {
	             console.log('exec error: ' + error);
	        }
	    });
	return res.send('Hello World!')
 })

app.listen(port, () => console.log(`Example app listening at http://localhost:${port}`))