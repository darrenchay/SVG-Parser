// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX
    $('#show-attr-btn').prop('disabled', true);

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/loadFiles',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            loadFileLogTable(data);
            loadDropdownData(data);
            /* $('#file-table-body').append('<tr> \
                                            <td><img src="' + data.getFileName + '" href="' + data.getFileName + '" class="img-fluid" alt="Responsive image"></td> \
                                            <td><a href="' + data.getFileName + '">' + data.getFileName + '</a></td> \
                                            <td>' + data.getFileSize + '</td> \
                                            <td>' + data.getNumRects + '</td> \
                                            <td>' + data.getNumCircs + '</td> \
                                            <td>' + data.getNumPaths + '</td> \
                                            <td>' + data.getNumGroups + '</td> \
                                            </tr>'); */

            
            //We write the object to the console to show that the request was successful
            console.log("Successfully loaded table"); 
            //console.log(data);
        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#file-log-table').html('<tr><td colspan="7">No files</td></tr>');
            console.log(error); 
        }
    });

    /* 
     * FILE LOG PANEL FUNCTIONS
     */

    $(".custom-file-input").on("change", function() {
        var fileName = $(this).val().split("\\").pop();
        $(this).siblings(".custom-file-label").addClass("selected").html(fileName);
    });

    /* $('#upload-file-btn').click(function() {
        var fileInput = document.getElementById('uploadFileInput');
        if (!fileInput) {
            alert("Um, couldn't find the fileinput element.");
          }
          else if (!fileInput.files) {
            alert("This browser doesn't seem to support the `files` property of file inputs.");
          }
          else if (!fileInput.files[0]) {
            alert("Please select a file before clicking 'Load'");               
          }
          else {
            var file = fileInput.files[0];
            //var fr = new FileReader();
            //fr.onload = receivedText;
            //fr.readAsText(file);
            //fr.readAsDataURL(file);
            console.log(file);
            //appendNewSVGFile(file);
          }
    }); */


    /* 
     * SVG VIEW PANEL FUNCTIONS
     */

    $("#chooseSVGDropdown").change(function(){
        var selectedSVG = $(this).children("option:selected").html();
        $('#view-img').attr("src", selectedSVG);
        $('#view-img').attr("href", selectedSVG);
        console.log("Selected: " + selectedSVG);
        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/loadFileData',   //The server endpoint we are connecting to
            data: {
                fileName: selectedSVG
            },
            success: function (data) {
                console.log("Title: " + data.title);
                console.log("Desc: " + data.desc);

                loadInfoSVG(data);
                
            },
            fail: function(error) {
                // Non-200 return, do something with error
                //$('#file-log-table').html('<tr><td colspan="7">No files</td></tr>');
                console.log(error); 
            }
    
        });
    });

    $("#selectShapeAttribute").change(function(){
        var selectedSVG = $(this).children("option:selected").html();
        $('#show-attr-btn').prop('disabled', false);
        console.log("Selected: " + selectedSVG);
        /* $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/loadFileData',   //The server endpoint we are connecting to
            data: {
                fileName: selectedSVG
            },
            success: function (data) {
                console.log("Title: " + data.title);
                console.log("Desc: " + data.desc);

                //loadInfoSVG(data);
                
            },
            fail: function(error) {
                // Non-200 return, do something with error
                //$('#file-log-table').html('<tr><td colspan="7">No files</td></tr>');
                console.log(error); 
            }
    
        }); */
    });

    /* Scaling circle */
    $(document).on('input', '#scale-circ-range', function() {
        $('#circ-range-lbl').html("Scale Circle:" + $(this).val());
        //console.log("val: " + $(this).val());
    });

    /* Updating scale circle */
    $('#scale-circle-btn').click(function(){
        var scaleVal = $('#scale-circ-range').val();
        console.log("val: " + scaleVal);
    });

    /* Scaling rect */
    $(document).on('input', '#scale-rect-range', function() {
        $('#rect-range-lbl').html("Scale Rectangle:" + $(this).val());
        //console.log("val: " + $(this).val());
    });

    /* Updating scale rect */
    $('#scale-rect-btn').click(function(){
        var scaleVal = $('#scale-rect-range').val();
        console.log("val: " + scaleVal);
    });
    
    document.getElementById('edit-title').onclick = function () {
        document.getElementById('titleInput').readOnly = false;
        $("#titleInput").addClass("border border-dark");
        document.getElementById('edit-title').style.display = "none";
        document.getElementById('confirm-edit-title').style.display = "block";
        console.log("Title can now be edited");
    };
    
    document.getElementById('confirm-edit-title').onclick = function() {
        document.getElementById('titleInput').readOnly = true;
        $("#titleInput").removeClass("border border-dark");
        document.getElementById('edit-title').style.display = "block";
        document.getElementById('confirm-edit-title').style.display = "none";
        console.log("Title saved");
    } 
    
    document.getElementById('edit-desc').onclick = function () {
        document.getElementById('descInput').readOnly = false;
        $("#descInput").addClass("border border-dark");
        document.getElementById('edit-desc').style.display = "none";
        document.getElementById('confirm-edit-desc').style.display = "block";
        console.log("Desc can now be edited");
    };
    
    document.getElementById('confirm-edit-desc').onclick = function() {
        document.getElementById('descInput').readOnly = true;
        $("#descInput").removeClass("border border-dark");
        document.getElementById('edit-desc').style.display = "block";
        document.getElementById('confirm-edit-desc').style.display = "none";
        console.log("Desc saved");
    } 


    /* 
     * MODAL FUNCTIONS
     */


    /* 
     * SHOW ATTRIBUTE MODAL FUNCTIONS
     */

    $('#create-svg-modal-btn').click(function() {
        var fileName = $('#fileName').val();
        if(!fileName) {
            console.log("invalid file name");
            alert("No file name");

        } else {
            $.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/createFile',   //The server endpoint we are connecting to
                data: {
                    fileName: fileName,
                    titleDesc: JSON.stringify(convertTitleAndDesc("", ""))
                },
                success: function (data) {
                    console.log("created file successfully");  
                    location.reload();
                    /* $.ajax({
                        type: 'get',            //Request type
                        dataType: 'json',       //Data type - we will use JSON for almost everything 
                        url: '/loadFiles',   //The server endpoint we are connecting to
                        success: function (data) {
                            loadFileLogTable(data);
                            loadDropdownData(data);
                            
                            //We write the object to the console to show that the request was successful
                            console.log("Successfully loaded table"); 
                            //console.log(data);
                        },
                        fail: function(error) {
                            console.log(error); 
                        }
                    });       */         
                },
                fail: function(error) {
                    console.log(error); 
                }
            });
            $('#create-svg-modal').modal('toggle');
            console.log("Created File: " + fileName + ".svg");
            //location.reload();
        }
    });

    /* Display the right attribute modal for the specific component */
    $(document).on("click", ".show-att-btn", function() {
        var selectedShape = $('#selectShapeAttribute').children("option:selected").html();
        console.log(selectedShape);

        var componentParts = selectedShape.split(" ");
        var selectedSVG = $('#chooseSVGDropdown').children("option:selected").html();
        console.log(selectedSVG);

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/loadAttributes',   //The server endpoint we are connecting to
            data: {
                type: componentParts[0],
                index: componentParts[1] - 1,
                fileName: selectedSVG
            },
            success: function (data) {
                console.log(data.shape);
                console.log(data.attrList);
                //loadAttributesList(data.attrList, data.shape, data.typeRet);
                var attrList = data.attrList;
                var shape = data.shape;
                var type = data.typeRet;
                $('#addAtributeInModal').html(' ');
    
                if(type == "Circle") {
                $('#addAtributeInModal').html('<h6><b> Shape Data </b></h6>');
                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">CX:</span>\
                                    </div>\
                                    <input type="number" id="cx-input-attr" min="0" class="form-control" value="' + shape.cx + '">\
                                </div>\
                            </div>\
                        </div>');
                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">CY:</span>\
                                    </div>\
                                    <input type="number" id="cy-input-attr" min="0" class="form-control" value="' + shape.cy + '">\
                                </div>\
                            </div>');
                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">Radius:</span>\
                                    </div>\
                                    <input type="number" id="r-input-attr" min="0" class="form-control" value="' + shape.r + '">\
                                </div>\
                            </div>\
                        </div>');
                } else if(type == "Rectangle") {
                    $('#addAtributeInModal').html('<h6><b> Shape Data </b></h6>');
                    $('#addAtributeInModal').append('<div class="row">\
                                                        <div class="col">\
                                                            <div class="input-group mb-3">\
                                                                <div class="input-group-prepend">\
                                                                    <span class="input-group-text">X:</span>\
                                                                </div>\
                                                                <input type="number" id="x-input-attr" min="0" class="form-control" value="' + shape.x + '">\
                                                            </div>\
                                                        </div>\
                                                    </div>');
                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">Y:</span>\
                                    </div>\
                                    <input type="number" id="y-input-attr" min="0" class="form-control" value="' + shape.y + '">\
                                </div>\
                            </div>\
                        </div>');
                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">Width:</span>\
                                    </div>\
                                    <input type="number" id="width-input-attr" min="0" class="form-control" value="' + shape.w + '">\
                                </div>\
                            </div>\
                        </div>');

                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">Height:</span>\
                                    </div>\
                                    <input type="number" id="height-input-attr" min="0" class="form-control" value="' + shape.h + '">\
                                </div>\
                            </div>\
                        </div>');
                } else if (type == "Path") {
                    $('#addAtributeInModal').html('<h6><b> Shape Data </b></h6>');
                    $('#addAtributeInModal').append('<div class="row">\
                            <div class="col">\
                                <div class="input-group mb-3">\
                                    <div class="input-group-prepend">\
                                        <span class="input-group-text">Data:</span>\
                                    </div>\
                                    <input type="text" id="data-input-attr" maxlength="255" class="form-control" value="' + shape.d + '">\
                                </div>\
                            </div>\
                        </div>');
                }

                $('#addNewAttForm').html('<div class="row">\
                                                    <div class="col-4">\
                                                        <label><b>Name</b></label>\
                                                    </div>\
                                                    <div class="col-1">\
                                                        <label><b>:</b></label>\
                                                    </div>\
                                                    <div class="col-4">\
                                                        <label><b>Value</b></label>\
                                                    </div>\
                                                </div>');

                for(var i = 0; i < attrList.length; i++) {
                    $('#addNewAttForm').append('<div class="row">\
                                                <input type="text" readonly class="form-control-plaintext col-4" id="' + attrList[i].name + '" value="'+ attrList[i].name +'">\
                                                <label class="col-1"> : </label>\
                                                <input type="text" readonly class="form-control-plaintext col-4" id="value-' + attrList[i].name  + '" value="'+ attrList[i].value +'">\
                                            <div class="col-3">\
                                                <button type="button" id="' + attrList[i].name  + '--edit-btn" class="btn btn-outline-secondary edit-attr-btns ">Edit</button>\
                                                <button type="button" id="' + attrList[i].name  + '--save-btn" class="btn btn-success save-edit-attr-btns ">Save</button>\
                                            </div>\
                                        </div>');
                }

                $('#addNewAttForm').append('<button type="button" id="add-attribute-btn" class="btn btn-info">Add Attribute</button>');
                $("#add-attribute-btn").click(function() {
                    console.log("clicked add");
                    $('#addNewAttForm').append('<div class="row">\
                                                        <input type="text" id="tempNameInput" class="form-control-plaintext col-4 border border-dark">\
                                                        <label class="col-1"> : </label>\
                                                        <input type="text" id="tempValueInput" class="form-control-plaintext col-4 border border-dark">\
                                                        <div class="col-3">\
                                                            <button type="button" id="tempEditAttBtnID" style="display: none" class="btn btn-outline-secondary edit-attr-btns ">Edit</button>\
                                                            <button type="button" id="tempSaveAttBtnID" style="display: block" class="btn btn-success save-edit-attr-btns ">Save</button>\
                                                        </div>\
                                                    </div>');
                    $('#add-attribute-btn').prop('disabled', true);
                });

                $('#save-attribute-changes').click(function() {
                    var selectedShape = $('#selectShapeAttribute').children("option:selected").html();
                    console.log(selectedShape);
                    var selectedSVG = $('#chooseSVGDropdown').children("option:selected").html();
                    var componentParts = selectedShape.split(" ");

                    let JSONstring;
                    if(componentParts[0] == "Rectangle") {
                        let x = $('#x-input-attr').val();
                        let y = $('#y-input-attr').val();
                        let h = $('#height-input-attr').val();
                        let w = $('#width-input-attr').val();
                        //console.log(x + "," + y + "," + w + "," + h);
                        let JSONx = '{"name":"x","value":"' + x +'"}';
                        let JSONy = '{"name":"y","value":"' + y +'"}';
                        let JSONw = '{"name":"width","value":"' + w +'"}';
                        let JSONh = '{"name":"height","value":"' + h +'"}';
                        JSONstring = '[' + JSONx + "," + JSONy + "," + JSONw + "," + JSONh + "]";
                        
                    } else if (componentParts[0] == "Circle") {
                        let cx = $('#cx-input-attr').val();
                        let cy = $('#cy-input-attr').val();
                        let r = $('#r-input-attr').val();
                        let JSONcx = '{"name":"cx","value":"' + cx +'"}';
                        let JSONcy = '{"name":"cy","value":"' + cy +'"}';
                        let JSONr = '{"name":"r","value":"' + r +'"}';
                        JSONstring = '[' + JSONcx + "," + JSONcy + "," + JSONr + "]";
                    } else if (componentParts[0] == "Path") {
                        let path = $('#data-input-attr').val();
                        let JSONpath = '{"name":"d","value":"' + path +'"}';
                        JSONstring = '[' + JSONpath + "]";
                    } else {
                        JSONstring = '[]';
                    }

                    console.log(JSONstring)
                    $.ajax({
                        type: 'get',            //Request type
                        dataType: 'json',       //Data type - we will use JSON for almost everything 
                        url: '/saveAttr',   //The server endpoint we are connecting to
                        data: {
                            type: componentParts[0],
                            index: componentParts[1] - 1,
                            fileName: selectedSVG,
                            JSONdata: JSONstring
                        },
                        success: function(data) {
                            returnVal = data.val;
                            console.log(data.val);
                            if(returnVal == 0) {
                                location.reload();
                            } else {
                                alert("Wrong data");
                            }
                        }
                    });
                    //location.reload();
                });
                console.log(data.attrList);
            },
            fail: function(error) {
                console.log(error); 
            }
    
        });
        

        /* $(this).parents("tr").find("th").each(function() {
            var componentName = $(this).text();
            var componentParts = componentName.split(' ');
            $('#showAttrModal').html("Attribute List of " + componentName);
            console.log(componentName + "parts: " + componentParts[0] + "::" + componentParts[1]);
            
        }) */
    });
    

    $(document).on("click", ".edit-attr-btns", function() {

        let temp = this.id;
        let data = temp.split('--');
        let name = data[0];

        /* Making texts editable */
        document.getElementById('value-' + name).readOnly = false;
        $("#value-"+ name).addClass("border border-dark");

        /* Hiding edit btn, showing save btn */
        document.getElementById(this.id).style.display = "none";
        document.getElementById(name + "--save-btn").style.display = "block";
        $('#add-attribute-btn').prop('disabled', true);
        console.log(name+ " can now be edited");
        
    });

    $(document).on("click", ".save-edit-attr-btns", function() {
        let name;
        let val;
        if(this.id == "tempSaveAttBtnID") {
            name = $('#tempNameInput').val();
            val = $('#tempValueInput').val();
            //console.log("new: [" + name);
        } else {
            let temp = this.id;
            let data = temp.split('--');
            name = data[0];
            val = $('#value-'+ name).val();
        }

        console.log(name + "," + val);
        let JSONstring = '[{"name":"' + name + '","value":"' + val +'"}]';

        var selectedShape = $('#selectShapeAttribute').children("option:selected").html();
        console.log(selectedShape);
        var selectedSVG = $('#chooseSVGDropdown').children("option:selected").html();
        var componentParts = selectedShape.split(" ");

        if(this.id == "tempSaveAttBtnID") {
            //console.log("here");
            if(document.getElementById('tempValueInput').value.length == 0 || document.getElementById('tempNameInput').value.length == 0 ) {
                alert("Please enter a name and value");
            } else {
                let returnVal;
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/saveAttr',   //The server endpoint we are connecting to
                    data: {
                        type: componentParts[0],
                        index: componentParts[1] - 1,
                        fileName: selectedSVG,
                        JSONdata: JSONstring
                    },
                    success: function(data) {
                        returnVal = data.val;
                        console.log("COMPLETED SAVING ATT");
                        console.log(data.val);
                        if(data.val < 0) {
                            alert("ERROR, wrong attribute name or value");
                        } else {
                            $('#tempSaveAttBtnID').attr("id", name + "--save-btn");
                            $('#tempNameInput').attr("id", name);
                            $('#tempValueInput').attr("id", "value-" + name);
                            $('#tempEditAttBtnID').attr("id", name + "--edit-btn");
                            //console.log(this.id);

                            /* Making texts readonly */
                            document.getElementById('value-' + name).readOnly = true;
                            $("#value-"+name).removeClass("border border-dark");
                            document.getElementById(name).readOnly = true;
                            $("#"+name).removeClass("border border-dark");

                            /* Hiding save btn, showing edit btn */
                            document.getElementById(name + "--save-btn").style.display = "none";
                            document.getElementById(name + "--edit-btn").style.display = "block";
                            $('#add-attribute-btn').prop('disabled', false);
                            
                            console.log(this.id + " can no longer be edited");
                        }
                    },
                    fail: function(error) {
                        //alert(error);
                        //console.log(error); 
                    }
            
                });
            }
        } else {
            if(document.getElementById('value-' + name).value.length == 0 || document.getElementById(name).value.length == 0 ) {
                alert("Please enter a name and value");
            } else {
                $.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/saveAttr',   //The server endpoint we are connecting to
                    data: {
                        type: componentParts[0],
                        index: componentParts[1] - 1,
                        fileName: selectedSVG,
                        JSONdata: JSONstring
                    },
                    success: function(data) {
                        returnVal = data.val;
                        console.log("COMPLETED SAVING ATT");
                        console.log(data.val);
                        if(data.val < 0) {
                            alert("ERROR, wrong attribute name or value");
                            
                        } else {
                            /* Making texts readonly */
                            document.getElementById('value-' + name).readOnly = true;
                            $("#value-"+name).removeClass("border border-dark");
                            document.getElementById(name).readOnly = true;
                            $("#"+name).removeClass("border border-dark");
                
                            /* Hiding save btn, showing edit btn */
                            document.getElementById(name + "--save-btn").style.display = "none";
                            document.getElementById(name + "--edit-btn").style.display = "block";
                            $('#add-attribute-btn').prop('disabled', false);
                            
                            console.log(this.id + " can no longer be edited");
                        }
                    },
                    fail: function(error) {
                        //console.log(error); 
                    }
            
                });
            }
        }
    });



    /* 
     * ADD CIRCLE 
     */

    $('#add-circle-modal-btn').click(function() {
        var cx = $('#cx-input').val();
        var cy = $('#cy-input').val();
        var r = $('#r-input').val();
        var unit = $('#unit-circ-input').val();

        console.log("Created circle with cx:" + cx + ", cy:" + cy + ", r:" + r + ", unit:" + unit);
        /* let circle;
        circle.append('cx', cx);
        circle.append('cy', cy);
        circle.append('r', r);
        circle.append('units', unit); */
        let string = '{"cx":' + cx + ',"cy":' + cy + ',"r":' + r + ',"units":"' + unit + '"}';
        console.log(string);
        /* let JSONString = string.stringify(string);
        console.log(JSONString); */

        var selectedSVG = $('#chooseSVGDropdown').children("option:selected").html();
        console.log(selectedSVG);
        $.ajax({
            type: 'get',            //Request type
            dataType: 'int',       //Data type - we will use JSON for almost everything 
            url: '/addShape',   //The server endpoint we are connecting to
            data: {
                JSONdata: string,
                type: 2,
                fileName: selectedSVG
            },
            success: function (data) {      
            },
            fail: function(error) {
                console.log(error); 
            }
    
        });

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/loadFiles',   //The server endpoint we are connecting to
            success: function (data) {
                loadFileLogTable(data);
                loadDropdownData(data);
                
                //We write the object to the console to show that the request was successful
                console.log("Successfully loaded table"); 
                //console.log(data);
            },
            fail: function(error) {
                console.log(error); 
            }
        });

        $('#add-circle-modal').modal('toggle');
        /* $('#contents-table-body').append('<tr>\
                            <th class="col-component">Circle 3</th>\
                            <td class="col-summary"> cx: ' + cx + unit + ' cy:' + cy + unit + ' r:' + r + unit + '</td>\
                            <td class="col-attributes">0\
                                <button class="btn btn-info btn-sm show-att-btn" data-toggle="modal" data-target="#show-attr-modal" type="button">Show</button>\
                            </td>\
                            </tr>'); */
        //alert("Sucessfully created circle!");
        location.reload(true);
    });


    /* ADD RECTANGLE MODAL */
    $('#add-rect-modal-btn').click(function() {
        var x = $('#x-input').val();
        var y = $('#y-input').val();
        var width = $('#width-input').val();
        var height = $('#height-input').val();
        var unit = $('#unit-rect-input').val();

        console.log("Created Rect with x:" + x + ", y:" + y + ", width:" + width + ", height:" + height + ", unit:" + unit);
        let string = '{"x":' + x + ',"y":' + y + ',"w":' + width + ',"h":' + height + ',"units":"' + unit + '"}';
        console.log(string);


        var selectedSVG = $('#chooseSVGDropdown').children("option:selected").html();
        console.log(selectedSVG);
        $.ajax({
            type: 'get',            //Request type
            dataType: 'int',       //Data type - we will use JSON for almost everything 
            url: '/addShape',   //The server endpoint we are connecting to
            data: {
                JSONdata: string,
                type: 1,
                fileName: selectedSVG
            },
            success: function (data) {
                
            },
            fail: function(error) {
                console.log(error); 
            }
    
        });

        $.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/loadFiles',   //The server endpoint we are connecting to
            success: function (data) {
                loadFileLogTable(data);
                loadDropdownData(data);
                
                //We write the object to the console to show that the request was successful
                console.log("Successfully loaded table"); 
                //console.log(data);
            },
            fail: function(error) {
                console.log(error); 
            }
        });      
        $('#add-rect-modal').modal('toggle');
        /* $('#contents-table-body').append('<tr>\
                            <th class="col-component">Rectangle 3</th>\
                            <td class="col-summary"> Upper left corner: x = ' + x + unit + ', y = ' + y + unit + ', Width:' + width + unit + ', Height:' + height + unit + '</td>\
                            <td class="col-attributes">0\
                                <button class="btn btn-info btn-sm show-att-btn" data-toggle="modal" data-target="#show-attr-modal" type="button">Show</button>\
                            </td>\
                            </tr>'); */
        //alert("Sucessfully created rectangle!");
        location.reload(true);

    });

});

/* Adds a new row to the file log table for the file input */
function appendNewSVGFile(file) {
    $('#file-table-body').append('<tr> \
                                <td class="img-cell"><img src="' + file[0] + '" href="' + file[0] + '" class="img-responsive img-thumbnail image" alt="Responsive image"></td> \
                                <td><a href="' + file[0] + '">' + file[0] + '</a></td> \
                                <td>' + file[1] + 'KB</td> \
                                <td>' + file[2] + '</td> \
                                <td>' + file[3] + '</td> \
                                <td>' + file[4] + '</td> \
                                <td>' + file[5] + '</td> \
                                </tr>');

}

/* generates the file log table */
function loadFileLogTable(data) {
    $('#file-table-body').html('');
    if(data.fileList.length === 0) {
        console.log("No files");
        $('#file-table-body').append('<tr> <td colspan= "7" class="align-middle"><b>No files to show</b></td> </tr>');
    }
    console.log(data.fileList);
    for(var i = 0; i < data.fileList.length; i++) {
        /* console.log(data.fileList[i]) */
        appendNewSVGFile(data.fileList[i]);
    }
} 

/* Loads all options in the dropdown upon page load */
function loadDropdownData(data) {
    $('#chooseSVGDropdown').html('');
    let files = data.fileList;
    for(var i = 0; i < files.length; i++) {
        $('#chooseSVGDropdown').append('<option value="' + files[i][0] + '">' + files[i][0] + '</option>');
    }
    $('#chooseSVGDropdown').append('<option value="" disabled selected hidden>Choose an SVG file</option>');
    console.log("created dropdown with " + files.length + " items");
}

/* Load data into svg view panel */
function loadInfoSVG(data) {
    /* Resets title and desc input */
    $('#titleInput').val('');
    $('#descInput').val('');
    $('#contents-table-body').html('');

    $('#selectShapeAttribute').html('');
    /* for(var i = 0; i < files.length; i++) {
        $('#selectShapeAttribute').append('<option value="' + files[i][0] + '">' + files[i][0] + '</option>');
    } */
    $('#selectShapeAttribute').append('<option value="" disabled selected hidden>Select Shape</option>');
    $('#selectShapeAttribute').append('<option value="svg-select">SVG</option>');

    if(data.title === "") {
        $('#titleInput').attr('placeholder', 'No Title');
    } else {
        $('#titleInput').val(data.title);
    }
    if(data.desc === "") {
        $('#descInput').attr('placeholder', 'No Description');
    } else {
        $('#descInput').val(data.desc);
    }

    for(var i = 0; i < data.rects.length; i++) {
        let rectangle = data.rects[i];
        $('#contents-table-body').append('<tr style="text-align: center">\
                                            <th class="col-component">Rectangle ' + (i - (-1)) + '</th>\
                                            <td class="col-summary">Upper left corner: x = ' + rectangle.x + rectangle.units + ', y = ' + rectangle.y + rectangle.units + ', Width: ' + rectangle.w + rectangle.units + ', Height: ' + rectangle.h + rectangle.units + '</td>\
                                            <td class="col-attributes"> ' + rectangle.numAttr + '</td>\
                                        </tr>');
        $('#selectShapeAttribute').append('<option value="Rectangle ' + (i - (-1)) + '">' + 'Rectangle ' + (i - (-1))+ '</option>');

    }

    for(var i = 0; i < data.circs.length; i++) {
        let circle = data.circs[i];
        $('#contents-table-body').append('<tr style="text-align: center">\
                                            <th class="col-component">Circle ' + (i - (-1)) + '</th>\
                                            <td class="col-summary">Center: x = ' + circle.cx + circle.units + ', y = ' + circle.cy + circle.units + ', radius = ' + circle.r + circle.units +'</td>\
                                            <td class="col-attributes"> ' + circle.numAttr + '</td>\
                                        </tr>');
        $('#selectShapeAttribute').append('<option value="Circle ' + (i - (-1)) + '">' + 'Circle ' + (i - (-1))+ '</option>');

    }

    for(var i = 0; i < data.paths.length; i++) {
        let path = data.paths[i];
        $('#contents-table-body').append('<tr style="text-align: center">\
                                            <th class="col-component">Path ' + (i - (-1)) + '</th>\
                                            <td class="col-summary">path data = ' + path.d + '</td>\
                                            <td class="col-attributes"> ' + path.numAttr + '</td>\
                                        </tr>');
        $('#selectShapeAttribute').append('<option value="Path ' + (i - (-1)) + '">' + 'Path ' + (i - (-1))+ '</option>');
    }

    for(var i = 0; i < data.groups.length; i++) {
        let group = data.groups[i];
        $('#contents-table-body').append('<tr style="text-align: center">\
                                            <th class="col-component">Group ' + (i - (-1)) + '</th>\
                                            <td class="col-summary"> ' + group.children + ' child elements</td>\
                                            <td class="col-attributes"> ' + group.numAttr + '</td>\
                                        </tr>');
        $('#selectShapeAttribute').append('<option value="Group ' + (i - (-1)) + '">' + 'Group ' + (i - (-1))+ '</option>');
    }
}

function loadAttributesList(attrList, shape, type) {
    console.log(type + " -- " + shape.numAttr + "/");
    $('#addAtributeInModal').html(' ');
    
    if(type == "Circle") {
    $('#addAtributeInModal').html('<h6><b> Shape Data </b></h6>');
        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">CX:</span>\
                        </div>\
                        <input type="number" id="cx-input-attr" min="0" class="form-control" value="' + shape.cx + '">\
                    </div>\
                </div>\
            </div>');
        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">CY:</span>\
                        </div>\
                        <input type="number" id="cy-input-attr" min="0" class="form-control" value="' + shape.cy + '">\
                    </div>\
                </div>');
        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">Radius:</span>\
                        </div>\
                        <input type="number" id="r-input-attr" min="0" class="form-control" value="' + shape.r + '">\
                    </div>\
                </div>\
            </div>');
    } else if(type == "Rectangle") {
        $('#addAtributeInModal').html('<h6><b> Shape Data </b></h6>');
        $('#addAtributeInModal').append('<div class="row">\
                                            <div class="col">\
                                                <div class="input-group mb-3">\
                                                    <div class="input-group-prepend">\
                                                        <span class="input-group-text">X:</span>\
                                                    </div>\
                                                    <input type="number" id="x-input-attr" min="0" class="form-control" value="' + shape.x + '">\
                                                </div>\
                                            </div>\
                                        </div>');
        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">Y:</span>\
                        </div>\
                        <input type="number" id="y-input-attr" min="0" class="form-control" value="' + shape.y + '">\
                    </div>\
                </div>\
            </div>');
        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">Width:</span>\
                        </div>\
                        <input type="number" id="width-input-attr" min="0" class="form-control" value="' + shape.w + '">\
                    </div>\
                </div>\
            </div>');

        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">Height:</span>\
                        </div>\
                        <input type="number" id="height-input-attr" min="0" class="form-control" value="' + shape.h + '">\
                    </div>\
                </div>\
            </div>');
    } else if (type == "Path") {
        $('#addAtributeInModal').html('<h6><b> Shape Data </b></h6>');
        $('#addAtributeInModal').append('<div class="row">\
                <div class="col">\
                    <div class="input-group mb-3">\
                        <div class="input-group-prepend">\
                            <span class="input-group-text">Data:</span>\
                        </div>\
                        <input type="text" id="data-input-attr" maxlength="255" class="form-control" value="' + shape.d + '">\
                    </div>\
                </div>\
            </div>');
    }

    $('#addNewAttForm').html('<div class="row">\
                                        <div class="col-4">\
                                            <label><b>Name</b></label>\
                                        </div>\
                                        <div class="col-1">\
                                            <label><b>:</b></label>\
                                        </div>\
                                        <div class="col-4">\
                                            <label><b>Value</b></label>\
                                        </div>\
                                    </div>');

    for(var i = 0; i < attrList.length; i++) {
        $('#addNewAttForm').append('<div class="row">\
                                    <input type="text" readonly class="form-control-plaintext col-4" id="' + attrList[i].name + '" value="'+ attrList[i].name +'">\
                                    <label class="col-1"> : </label>\
                                    <input type="text" readonly class="form-control-plaintext col-4" id="value-' + attrList[i].name  + '" value="'+ attrList[i].value +'">\
                                <div class="col-3">\
                                    <button type="button" id="' + attrList[i].name  + '--edit-btn" class="btn btn-outline-secondary edit-attr-btns ">Edit</button>\
                                    <button type="button" id="' + attrList[i].name  + '--save-btn" class="btn btn-success save-edit-attr-btns ">Save</button>\
                                </div>\
                            </div>');
    }

    $('#addNewAttForm').append('<button type="button" id="add-attribute-btn" class="btn btn-info">Add Attribute</button>');
    $("#add-attribute-btn").click(function() {
        console.log("clicked add");
        $('#addNewAttForm').append('<div class="row">\
                                            <input type="text" id="tempNameInput" class="form-control-plaintext col-4 border border-dark">\
                                            <label class="col-1"> : </label>\
                                            <input type="text" id="tempValueInput" class="form-control-plaintext col-4 border border-dark">\
                                            <div class="col-3">\
                                                <button type="button" id="tempEditAttBtnID" style="display: none" class="btn btn-outline-secondary edit-attr-btns ">Edit</button>\
                                                <button type="button" id="tempSaveAttBtnID" style="display: block" class="btn btn-success save-edit-attr-btns ">Save</button>\
                                            </div>\
                                        </div>');
        $('#add-attribute-btn').prop('disabled', true);
    });
    /* $('#addNewAttrForm').append('<div class="row">\
                                    <div class="col-4">\
                                        <input type="text" readonly class="form-control-plaintext">\
                                    </div>\
                                    <div class="col-1">\
                                        <label>  </label>\
                                    </div>\
                                    <div class="col-4">\
                                        <input type="text" readonly class="form-control-plaintext">\
                                    </div>\
                                </div>\
                                <button type="button" id="add-attribute-btn" class="btn btn-info">Add Attribute</button>'); */
}

function convertTitleAndDesc(title, desc) {
    let JSONString = '{"title":"' + title + '","descr":"' + desc + '"}';
    return JSONString;
}
