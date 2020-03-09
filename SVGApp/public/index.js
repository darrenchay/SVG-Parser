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
            $('#blah').html("On page load, received string '"+data.foo+"' from server");

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
            console.log(data); 
            //$('#file-log-table').html('<p>No files</p>');

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            $('#file-log-table').html('<p>No files</p>');
            console.log(error); 
        }
    });

    //Populating file log panel
    document.getElementById('file-log-table');

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
});
