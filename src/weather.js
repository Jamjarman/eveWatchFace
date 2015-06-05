//listen for when watchface is opened
Pebble.addEventListener('ready', function(e){
  console.log('PebbleKit JS ready!');
  //get weather
  getWeather();
});
//listen for when an appmessage is received
Pebble.addEventListener('appmessage', function(e){
  console.log('AppMessage received!');
  getWeather();
});

var xhrRequest=function(url, type, callback){
  var xhr=new XMLHttpRequest();
  xhr.onload=function(){
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
  
function locationSuccess(pos){
  //we will request weather here
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat='+pos.coords.latitude + '&lon=' + pos.coords.longitude;
  xhrRequest(url, 'GET', function(responseText){
    console.log(JSON.parse(responseText));
    //responesTex contains a json object with weather info
    var json=JSON.parse(responseText);
    //Temp in kelvin needs to be adjusted
    var temperature=Math.round((json.main.temp - 273.15)*1.8+32);
    console.log('Temperature is '+ temperature);
    //conditions
    var conditions = json.weather[0].main;
    console.log('Conditions are '+conditions);
    var dictionary={
      'KEY_TEMPERATURE': temperature,
      'KEY_CONDITIONS': conditions
    };
    
    Pebble.sendAppMessage(dictionary, function(e){
      console.log('Weather info sent to Pebble successfully!');
    }, function(e){
      console.log('Error sending weather info to Pebble!');
    });
  });
}

function locationError(err){
  console.log('Error requesting location!');
}

function getWeather(){
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, {timeout: 15000, maximumAge: 60000});
}

