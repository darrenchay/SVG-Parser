'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sending all file and the data for file log panel and dropdown
app.get('/loadFiles', function(req , res){

  let sharedLib = ffi.Library('./libsvgparse', {
    'readSVGtoJSON' : [ 'string', [ 'string', 'string' ] ],
  });

  const fs = require('fs');

  let fileListArray = [];
  /* Reading all svg files into items */
  fs.readdir('uploads/', function(err, items) {
    //console.log(items);

    /* Creating the array of JSON objects for each SVG file */
    for(var i = 0; i < items.length; i++) {
      let SVGasJSON = sharedLib.readSVGtoJSON('uploads/' + items[i], 'svg.xsd');

      if(SVGasJSON == "{}") {
        continue;
      }

      let currFileData = JSON.parse(SVGasJSON);

      //console.log(SVGasString);
      //console.log(currFileData);

      let stats = fs.statSync('uploads/' + items[i]);
      let size = stats.size/1000;

      let fileData = [items[i], size, currFileData.numRect, currFileData.numCirc, currFileData.numPaths, currFileData.numGroups];
      //console.log("ARRAY");
      //console.log(fileData);
      fileListArray.push(fileData); 
    }
    //console.log("Array:");
    console.log(fileListArray);
    res.send({
      fileList: fileListArray
    });
  });

  
});

app.get('/loadFileData', function(req, res) {
  let sharedLib = ffi.Library('./libsvgparse', {
    'SVGdetailsToJSON' : [ 'string', [ 'string', 'string' ] ],
  });

  let fileName = req.query.fileName;
  console.log(fileName);

  let titleAndDescAsJSON = sharedLib.SVGdetailsToJSON('uploads/' + fileName, 'svg.xsd');
  console.log(titleAndDescAsJSON);

  let fileData = JSON.parse(titleAndDescAsJSON);
  console.log(fileData.desc);
  res.send({
    title: fileData.title,
    desc: fileData.desc
  });

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);