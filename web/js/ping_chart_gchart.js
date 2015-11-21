/**
 * This file is only for testing
 * Gcharts can't hanle these amounts of data
 */


google.load('visualization', '1', {packages: ['corechart', 'line']});
google.load('visualization', '1', {'packages':['annotationchart']});

getJSON('http://192.168.1.100:8000/api/pings').then(function(data) {
//getJSON('http://localhost:8000/api/pings').then(function(data) {
    console.log(data);
    graph_data = [];

    //data.length-10000
    for(var i =  0; i < data.length; ++i)
    {
        graph_data.push([new Date(data[i].Time), data[i].Status])
    }

    function draw() {
      var data = new google.visualization.DataTable();
      data.addColumn('datetime', 'X');
      data.addColumn('number', 'Status');

      data.addRows(graph_data);

      var options = {
        hAxis: {
          title: 'Time'
        },
        vAxis: {
          title: 'Popularity'
        },
        colors: ['green', 'red'],
        explorer: {axis: 'horizontal', keepInBounds: true},
//        hAxis: {maxValue: 23, minValue:0, viewWindow:{max: 23}}
      };

      var chart = new google.visualization.LineChart(document.getElementById('ping_chart'));
//      var chart = new google.visualization.AnnotationChart(document.getElementById('ping_chart'));

      chart.draw(data, options);
    }

    google.setOnLoadCallback(draw);
}, function(status) {
  alert('Something went wrong.');
});





