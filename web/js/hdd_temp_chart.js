google.load("visualization", "1.1", {packages:["timeline"]});

var drive_connection = [];

function draw() {
    getJSON('http://s.tivvit.cz:8000/api/hdd-temps').then(function (data) {
//        getJSON('http://localhost:8000/api/hdd-temps').then(function(data) {
        console.log(data);
        var graph_data = [];

        var stats = data.stats;
        for (var i = 0; i < stats.length; ++i) {
            var date = new Date(stats[i].time * 1000);

            var drive_temps = [date]
            for (var j = 0; j < stats[i].drives.length; ++j) {
                if(stats[i].drives[j].temp == 0)
                    drive_temps.push(null);
                else
                    drive_temps.push(stats[i].drives[j].temp);
            }
            graph_data.push(drive_temps);
        }

        var drive_names = ["time"];

        for (var i = 0; i < data.drives.length; ++i) {
            var drive = data.drives[i];
            var drive_name = drive.manufacturer + "-" + drive.sn + " [" + drive.model + "]" +  " (" + drive["last-mnt"] + ")";
            drive_names.push(drive_name);
            drive_connection.push([drive_name, new Date(drive.first * 1000), new Date(drive.last * 1000)])
        }

//    console.log(graph_data);
        console.log(drive_connection);

        new Dygraph(document.getElementById("hdd-temp-chart"), graph_data, {
            labels: drive_names,
            title: 'HDD temps',
            showRangeSelector: true,
            showRoller: true,
            rollPeriod: 10,
//        drawPoints: true,
            strokeWidth: 1,
            dateWindow: [Date.now() - 1000 * 60 * 60 * 6 , Date.now()],
            highlightCircleSize: 6,
            yRangePad: 50.,
        });

//    function draw() {
        var container = document.getElementById('hdd-connect-chart');
        var chart = new google.visualization.Timeline(container);
        var dataTable = new google.visualization.DataTable();

        dataTable.addColumn({ type: 'string', id: 'Name' });
        dataTable.addColumn({ type: 'datetime', id: 'Start' });
        dataTable.addColumn({ type: 'datetime', id: 'End' });

        dataTable.addRows(drive_connection);

        var options = {
            "height": 450
        };

        chart.draw(dataTable, options);
//    }
    }, function (status) {
        alert('Something went wrong.');
    });
}





google.setOnLoadCallback(draw);



