getJSON('http://s.tivvit.cz:8000/api/pings').then(function(data) {
//    console.log(data);
    var graph_data = [];

    for(var i = 0; i < data.length; ++i)
    {
        var date = new Date(data[i].time * 1000);

        if(data[i].status)
            graph_data.push([date, null, 0.5]);
        else
            graph_data.push([date, 1, null]);
    }

//    console.log(graph_data);

    new Dygraph(document.getElementById("ping-chart"), graph_data, {
        labels: ["time", "ok", "err"],
        colors: ["green", "red"],
        legend: 'always',
        title: 'Pinger',
        height: 175,
        showRangeSelector: true,
        drawPoints: true,
        fillGraph: true,
        strokeWidth: 2,
        dateWindow: [Date.now()-1000*60*60*6 , Date.now()],
        yRangePad: 100.,
        rangeSelectorHeight: 20,
    });

}, function(status) {
  alert('Something went wrong.');
});
