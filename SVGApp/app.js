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
 
  let uploadFile = req.files.uploadSVG;

  /* console.log(uploadFile);
  console.log("TESTING"); */
 
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
    console.log("Created Array");
    //console.log(fileListArray);
    res.send({
      fileList: fileListArray
    });
  });

  
});

app.get('/loadFileData', function(req, res) {
  let sharedLib = ffi.Library('./libsvgparse', {
    'SVGdetailsToJSON' : [ 'string', [ 'string', 'string' ] ],
    'getRectJSONlist' : [ 'string', [ 'string', 'string' ] ],
    'getCircJSONlist' : [ 'string', [ 'string', 'string' ] ],
    'getPathJSONlist' : [ 'string', [ 'string', 'string' ] ],
    'getGroupJSONlist' : [ 'string', [ 'string', 'string' ] ],
  });

  let fileName = req.query.fileName;
  console.log(fileName);

  let titleAndDescAsJSON = sharedLib.SVGdetailsToJSON('uploads/' + fileName, 'svg.xsd');
  console.log(titleAndDescAsJSON);

  let rectListJSON = sharedLib.getRectJSONlist('uploads/' + fileName, 'svg.xsd');
  let rectList = JSON.parse(rectListJSON);
  console.log(rectList);

  let circListJSON = sharedLib.getCircJSONlist('uploads/' + fileName, 'svg.xsd');
  let circList = JSON.parse(circListJSON);
  console.log(circList);

  let pathListJSON = sharedLib.getPathJSONlist('uploads/' + fileName, 'svg.xsd');
  let pathList = JSON.parse(pathListJSON);
  console.log(pathList);

  let groupListJSON = sharedLib.getGroupJSONlist('uploads/' + fileName, 'svg.xsd');
  let groupList = JSON.parse(groupListJSON);
  console.log(groupList);


  let fileData = JSON.parse(titleAndDescAsJSON);
  console.log(fileData.desc);
  res.send({
    title: fileData.title,
    desc: fileData.desc,
    rects: rectList,
    circs: circList,
    paths: pathList,
    groups: groupList
  });

});

app.get('/loadAttributes', function(req, res) {
  let sharedLib = ffi.Library('./libsvgparse', {
    'getAttrJSONlist' : [ 'string', [ 'string', 'string', 'int', 'int' ] ]
  });

  let type = req.query.type;
  let index = req.query.index;
  let fileName = req.query.fileName;

  console.log(type + " // " +index);
  console.log(fileName);

  let attrListAsString;
  let shapeData;
  if(type === "Rectangle") {
    attrListAsString = sharedLib.getAttrJSONlist('uploads/' + fileName, 'svg.xsd', index, 1);
  } else if(type === "Circle") {
    attrListAsString = sharedLib.getAttrJSONlist('uploads/' + fileName, 'svg.xsd', index, 2);
  } else if(type === "Path") {
    attrListAsString = sharedLib.getAttrJSONlist('uploads/' + fileName, 'svg.xsd', index, 3);
  } else if(type === "Group") {
    attrListAsString = sharedLib.getAttrJSONlist('uploads/' + fileName, 'svg.xsd', index, 4);
  } else if (type === "SVG") {
      attrListAsString = sharedLib.getAttrJSONlist('uploads/' + fileName, 'svg.xsd', 0, 0);
  }

  console.log("retrieved JSON");
  //console.log(attrListAsString);

  let retAttList;
  let shapeRet;
  shapeData = JSON.parse(attrListAsString);
  if(type === "SVG" || type === "Group") {
    shapeRet = "[]";
    retAttList = shapeData[0];
  } else {
    shapeRet = shapeData[0];
    retAttList = shapeData[1];
  }

  console.log("Shape data: ");
  console.log(shapeRet);
  console.log("Shape attributes: ");
  console.log(retAttList);

  res.send({
    shape: shapeRet,
    attrList: retAttList,
    typeRet: type
  });

});

app.get('/createFile', function(req, res) {
  let sharedLib = ffi.Library('./libsvgparse', {
    'writeJSONSVGtoSVGFile' : [ 'int', [ 'string', 'string', 'string' ] ],
  });

  let fileName = req.query.fileName;
  console.log(fileName);

  let titleDescJSON = req.query.titleDesc;
  console.log(titleDescJSON);

  let returnVal = sharedLib.writeJSONSVGtoSVGFile(titleDescJSON, 'uploads/' + fileName + ".svg", 'svg.xsd');
  console.log(returnVal);

  /* let titleAndDescAsJSON = sharedLib.SVGdetailsToJSON('uploads/' + fileName, 'svg.xsd');
  console.log(titleAndDescAsJSON);

  let fileData = JSON.parse(titleAndDescAsJSON);
  console.log(fileData.desc); */
  res.send({
    success: 1
  });

});

app.get('/addShape', function(req, res) {
  let sharedLib = ffi.Library('./libsvgparse', {
    'addShapeToSVGFile' : [ 'int', [ 'string', 'string', 'string', 'int' ] ],
  });

  let fileName = req.query.fileName;
  console.log(fileName);

  let data = req.query.JSONdata;
  console.log(data);

  let type = req.query.type;

  let returnVal = sharedLib.addShapeToSVGFile('uploads/' + fileName, 'svg.xsd', data, type);
  console.log(returnVal);

  /* let titleAndDescAsJSON = sharedLib.SVGdetailsToJSON('uploads/' + fileName, 'svg.xsd');
  console.log(titleAndDescAsJSON);

  let fileData = JSON.parse(titleAndDescAsJSON);
  console.log(fileData.desc); */
  res.send({
    success: returnVal
  });

});


app.get('/saveAttr', function(req, res) {
  let sharedLib = ffi.Library('./libsvgparse', {
    'writeExistingShapeFromJSON' : [ 'int', [ 'string', 'string', 'string', 'int', 'int' ] ],
    'writeAttrFromJSON' : [ 'int', [ 'string', 'string', 'int', 'int', 'string' ] ],

  });

  let fileName = req.query.fileName;
  console.log(fileName);

  let data = req.query.JSONdata;
  let JSONdata = JSON.parse(data);
  console.log(JSONdata);

  let typeRet = req.query.type;
  let type;
  if(typeRet == "Rectangle") {
    type = 1;
  } else if (typeRet == "Circle") {
    type = 2;
  } else if (typeRet == "Path") {
    type = 3;
  } else if (typeRet == "Group") {
    type = 4;
  } else if (typeRet == "SVG") {
    type = 0;
  }

  let returnVal;
  
  for(var i = 0; i < JSONdata.length; i++) {
    console.log("For: " + JSON.stringify(JSONdata[i]));
    returnVal = sharedLib.writeAttrFromJSON('uploads/' + fileName, 'svg.xsd', req.query.index, type, JSON.stringify(JSONdata[i]));
    console.log(returnVal);
  }

  res.send({
    val: returnVal
  });

});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);