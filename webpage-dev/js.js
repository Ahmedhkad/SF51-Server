function toggleCheckbox(x) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/" + x, true);
    xhr.send();
  }

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
 xhttp.onreadystatechange = function() {
   if (this.readyState == 4 && this.status == 200) {
     document.getElementById("temperaturec").innerHTML = this.responseText;
   }
 };
 xhttp.open("GET", "/temperaturec", true);
 xhttp.send();
 }, 3000 ) ;

function logoutButton() {
 var xhr = new XMLHttpRequest();
 xhr.open("GET", "/logout", true);
 xhr.send();
 setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}

function getReadings(){
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        console.log(myObj);
        var temp = myObj.temperature;
        var hum = myObj.humidity;
        gaugeTemp.value = temp;
        gaugeHum.value = hum;
      }
    }; 
    xhr.open("GET", "/readings", true);
    xhr.send();
  }