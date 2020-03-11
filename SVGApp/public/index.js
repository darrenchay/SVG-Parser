// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        //dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/loadFiles',   //The server endpoint we are connecting to
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#file-table-body').append(loadFileLogTable(data));
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
            console.log(data);
            
            //$('#file-log-table').html('<p>No files</p>');

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#file-log-table').html('<tr><td colspan="7">No files</td></tr>');
            console.log(error); 
        }
    });


    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    $(".custom-file-input").on("change", function() {
        var fileName = $(this).val().split("\\").pop();
        $(this).siblings(".custom-file-label").addClass("selected").html(fileName);
    });

    $("#chooseSVGDropdown").change(function(){
        var selectedSVG = $(this).children("option:selected").html();
        $('#view-img').attr("src", selectedSVG);
        console.log("Selected: " + selectedSVG);
    });
    
    $("#add-attribute-btn").click(function() {
        $('#addAtributeInModal').append('<div class="form-group row">\
                                        <label for="FillRect2" class="col-sm-2 col-form-label">New</label>\
                                        <div class="col-sm-10">\
                                        <input type="text" class="form-control" placeholder="Val" id="FillRect2">\
                                        </div>\
                                        </div>')
    })

    $('#create-svg-modal-btn').click(function() {
        var fileName = $('#fileName').val();
        if(!fileName) {
            console.log("invalid file name");

        } else {
            console.log("Created File: " + fileName + ".svg");
            $('#create-svg-modal').modal('toggle');
        }
    });

    $('#upload-file-btn').click(function() {
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

    $('#create-svg-modal-btn').submit(function() {
        'use strict';
        window.addEventListener('load', function() {
            // Fetch all the forms we want to apply custom Bootstrap validation styles to
            var forms = document.getElementsByClassName('needs-validation');
            // Loop over them and prevent submission
            var validation = Array.prototype.filter.call(forms, function(form) {
            form.addEventListener('submit', function(event) {
                if (form.checkValidity() === false) {
                event.preventDefault();
                event.stopPropagation();
                }
                form.classList.add('was-validated');
            }, false);
            });
        }, false);
    });

    $('#add-circle-modal-btn').click(function() {
        var cx = $('#cx-input').val();
        var cy = $('#cy-input').val();
        var r = $('#r-input').val();
        var unit = $('#unit-circ-input').val();

        console.log("Created circle with cx:" + cx + ", cy:" + cy + ", r:" + r + ", unit:" + unit);
        $('#add-circle-modal').modal('toggle');
        $('#contents-table-body').append('<tr>\
                            <th class="col-component">Circle 3</th>\
                            <td class="col-summary"> cx: ' + cx + unit + ' cy:' + cy + unit + ' r:' + r + unit + '</td>\
                            <td class="col-attributes">0\
                                <button class="btn btn-info btn-sm show-att-btn" data-toggle="modal" data-target="#show-attr-modal" type="button">Show</button>\
                            </td>\
                            </tr>');
        //alert("Sucessfully created circle!");
    });

    $('#add-rect-modal-btn').click(function() {
        var x = $('#x-input').val();
        var y = $('#y-input').val();
        var width = $('#width-input').val();
        var height = $('#height-input').val();
        var unit = $('#unit-rect-input').val();

        console.log("Created Rect with x:" + x + ", y:" + y + ", width:" + width + ", height:" + height + ", unit:" + unit);
        $('#add-rect-modal').modal('toggle');
        $('#contents-table-body').append('<tr>\
                            <th class="col-component">Rectangle 3</th>\
                            <td class="col-summary"> Upper left corner: x = ' + x + unit + ', y = ' + y + unit + ', Width:' + width + unit + ', Height:' + height + unit + '</td>\
                            <td class="col-attributes">0\
                                <button class="btn btn-info btn-sm show-att-btn" data-toggle="modal" data-target="#show-attr-modal" type="button">Show</button>\
                            </td>\
                            </tr>');
        //alert("Sucessfully created rectangle!");
    });

    /* Display the right attribute modal for the specific component */
    $(document).on("click", ".show-att-btn", function() {
        $(this).parents("tr").find("th").each(function() {
            var componentName = $(this).text();
            var componentParts = componentName.split(' ');
            $('#showAttrModal').html("Other attributes of " + componentName);
            console.log(componentName + "parts: " + componentParts[0] + "::" + componentParts[1]);
        })
    });
});

function appendNewSVGFile(file) {
    $('#file-table-body').append('<tr> \
                                <td><img src="' + file[0] + '" href="' + file[0] + '" class="img-responsive img-thumbnail" alt="Responsive image"></td> \
                                <td><a href="' + file[0] + '">' + file[0] + '</a></td> \
                                <td>' + file[1] + 'KB</td> \
                                <td>' + file[2] + '</td> \
                                <td>' + file[3] + '</td> \
                                <td>' + file[4] + '</td> \
                                <td>' + file[5] + '</td> \
                                </tr>');

}

function loadFileLogTable(data) {
    if(!data) {
        console.log("No files");
        $('#file-table-body').append('<tr> <td colspan= "7">No files </td> </tr>');
    }
    console.log(data.fileList);
    for(var i = 0; i < data.fileList.length; i++) {
        console.log("1:");
        console.log(data.fileList[i])
        appendNewSVGFile(data.fileList[i]);
    }
} 