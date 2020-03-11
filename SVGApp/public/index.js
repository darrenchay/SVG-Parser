// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/someendpoint',   //The server endpoint we are connecting to
        data: {
            file: "rects.svg",
            size: "1KB",
            rects: "1",
            circs: "2",
            paths: "5",
            groups: "4",
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */

            $('#file-table-body').append('<tr> \
                                            <td><img src="' + data.getFileName + '" href="' + data.getFileName + '" class="img-fluid" alt="Responsive image"></td> \
                                            <td><a href="' + data.getFileName + '">' + data.getFileName + '</a></td> \
                                            <td>' + data.getFileSize + '</td> \
                                            <td>' + data.getNumRects + '</td> \
                                            <td>' + data.getNumCircs + '</td> \
                                            <td>' + data.getNumPaths + '</td> \
                                            <td>' + data.getNumGroups + '</td> \
                                            </tr>');
            $('#file-table-body').append('<tr> \
                                            <td><img src="' + data.getFileName + '" href="' + data.getFileName + '" class="img-fluid" alt="Responsive image"></td> \
                                            <td><a href="' + data.getFileName + '">' + data.getFileName + '</a></td> \
                                            <td>' + data.getFileSize + '</td> \
                                            <td>' + data.getNumRects + '</td> \
                                            <td>' + data.getNumCircs + '</td> \
                                            <td>' + data.getNumPaths + '</td> \
                                            <td>' + data.getNumGroups + '</td> \
                                            </tr>');

            $('#file-table-body').append('<tr> \
                                            <td><img src="' + data.getFileName + '" href="' + data.getFileName + '" class="img-fluid" alt="Responsive image"></td> \
                                            <td><a href="' + data.getFileName + '">' + data.getFileName + '</a></td> \
                                            <td>' + data.getFileSize + '</td> \
                                            <td>' + data.getNumRects + '</td> \
                                            <td>' + data.getNumCircs + '</td> \
                                            <td>' + data.getNumPaths + '</td> \
                                            <td>' + data.getNumGroups + '</td> \
                                            </tr>');
            
            $('#file-table-body').append('<tr> \
                                            <td><img src="' + data.getFileName + '" href="' + data.getFileName + '" class="img-fluid" alt="Responsive image"></td> \
                                            <td><a href="' + data.getFileName + '">' + data.getFileName + '</a></td> \
                                            <td>' + data.getFileSize + '</td> \
                                            <td>' + data.getNumRects + '</td> \
                                            <td>' + data.getNumCircs + '</td> \
                                            <td>' + data.getNumPaths + '</td> \
                                            <td>' + data.getNumGroups + '</td> \
                                            </tr>');

            
            //We write the object to the console to show that the request was successful
            console.log("Successfully loaded table"); 
            console.log(data);
            
            //$('#file-log-table').html('<p>No files</p>');

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#file-log-table').html('<p>No files</p>');
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
        var selectedSVG = $(this).children("option:selected").val();
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
        console.log("Created File: " + fileName + ".svg");
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