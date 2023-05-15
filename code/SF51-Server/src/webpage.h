const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
    margin: 0;
    color: aliceblue;
    overflow: hidden;
    background: #202020;
    font-family: "Fira Sans", sans-serif;
    font-size: 12px;
    text-align: center;
    }
    .nav {
    display: flex;
    max-width: 70em;
    margin: auto;
    flex-direction: row-reverse;
    justify-content: space-between;
    }

    .header {
    font-size: 16px;
    }

    .temp {
    color: black;
    font-size: 20px;
    margin: 0.5em auto;
    max-width: 40em;
    padding: 10px;
    border: 3px solid;
    }
    .mqtt{
    color: inherit;
    }

    .container {
    display: flex;
    flex-direction: row;
    justify-content: center;
    align-items: center;
    }

    .box {
    background-color: rgb(57, 57, 58);
    width: 30em;
    margin: 0.5em;
    padding: 2vw;
    border: 3px solid;
    }

    .button {
    background-color: #04aa6d;
    font-family: "Source Sans Pro", sans-serif;
    color: white;
    font-size: 16px;
    padding: 6px 15px;
    margin-top: 5px;
    margin-right: 5px;
    border-radius: 5px;
    cursor: pointer;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
    }
    .button:hover {background-color: #046d1b;}
    .button:active {
    background-color: #ed0000;;
    box-shadow: 0 4px #666;
    transform: translateY(2px);
    }
    .ONLINE{
        color:green;
    }
    .OFFLINE{
        color:red;
    }
    </style>

</head>
<body>
   
    <div class="nav">
        <button class="button" onclick="logoutButton()">Logout</button>
        <p id="loading">Updating...</p>
        <button class="button" onclick="goOffline()">pingOffline</button>
    </div>
    <div class="header"> 
        <h1>SF51-Server</h1>
        <h3>Web Server Control</h3>
    </div>
    <p id="servertemp" class="temp"> Server Temperture: 
       <span id="temperaturec">--</span>
       <sup class="units">&deg;c</sup>
   </p>
   <p class="mqtt temp"> MQTT Server:
    <span id="mqttServer">---</span>
    </p>
    <div class="container">
        <div class="box">
            <p>LED State : <span id="StrongServerLEDid">---</span> </p>
            <p>Ping State : <span class="OFFLINE" id="StrongServerCid">---</span> </p>
            <p id="piningStrong">Pinging...</p>

            <button class="button" onmousedown="toggleCheckbox('onStrongServer');"
                ontouchstart="toggleCheckbox('onStrongServer');" onmouseup="toggleCheckbox('offStrongServer');"
                ontouchend="toggleCheckbox('offStrongServer');">Start StrongServer</button>
        </div>

        <div class="box">
             <p>LED State : <span id="CoolServerLEDid">---</span> </p>
            <p>Ping State : <span class="OFFLINE" id="CoolServerCid">---</span> </p>
            <p id="piningCool">Pinging...</p>

            <button class="button" onmousedown="toggleCheckbox('onCoolServer');"
                ontouchstart="toggleCheckbox('onCoolServer');" onmouseup="toggleCheckbox('offCoolServer');"
                ontouchend="toggleCheckbox('offCoolServer');">Start CoolServer</button>
        </div>
    </div>

<script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }

setInterval(function ( ) {
  document.getElementById("loading").style.visibility = "visible";
  setTimeout(() => {  document.getElementById("loading").style.visibility = "hidden"; }, 500);
    var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
       var myObj = JSON.parse(this.responseText);
        console.log(myObj);
        var Temperature = myObj.Temperature;
        var MQTTserver = myObj.MQTTserver;
        if (myObj.StrongLED == 0) {
            var StrongLEDc = "OFFLINE";
          }else if (myObj.StrongLED == 1){
         var StrongLEDc = "ONLINE";
          } 
        if (myObj.CoolLED == 0) {
            var CoolLEDc = "OFFLINE";
          }else if (myObj.CoolLED == 1){
         var CoolLEDc = "ONLINE";
          } 
        if (myObj.MQTTserver == 0) {
            var MqttState = "OFFLINE";
          }else if (myObj.MQTTserver == 1){
         var MqttState = "ONLINE";
          } 
        document.getElementById("mqttServer").className = MqttState;
        document.getElementById("mqttServer").innerHTML = MqttState;
        document.getElementById("temperaturec").innerHTML = Temperature;
        document.getElementById("StrongServerLEDid").innerHTML = StrongLEDc;
        document.getElementById("StrongServerLEDid").className = StrongLEDc;
        document.getElementById("CoolServerLEDid").innerHTML = CoolLEDc;
        document.getElementById("CoolServerLEDid").className = CoolLEDc;
        var NewValue = (Temperature - 0) * (0 - 220) / (60 - 0) + 220; 
        document.getElementById("servertemp").style.backgroundColor  = 'hsl(' + [NewValue, '100%', '50%'] + ')';
     }
   };
   xhttp.open("GET", "/api/sensors", true);
   xhttp.send();
   }, 6000 ) ;

   setInterval(function ( ) {
    document.getElementById("piningStrong").style.visibility = "visible";
  setTimeout(() => {  document.getElementById("piningStrong").style.visibility = "hidden"; }, 3000);
   var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("StrongServerCid").innerHTML = this.responseText;
      document.getElementById("StrongServerCid").className = this.responseText;
    }
  };
  xhttp.open("GET", "/StrongPing", true);
  xhttp.send();
  }, 8000 ) ;

  setInterval(function ( ) {
    document.getElementById("piningCool").style.visibility = "visible";
  setTimeout(() => {  document.getElementById("piningCool").style.visibility = "hidden"; }, 3000);
   var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("CoolServerCid").innerHTML = this.responseText;
      document.getElementById("CoolServerCid").className = this.responseText;
    }
  };
  xhttp.open("GET", "/CoolPing", true);
  xhttp.send();
  }, 11000 ) ;

function goOffline(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/goOFFLINE", true);
  xhr.send();
}
function logoutButton() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}
</script>
</body>
</html>
)rawliteral";

const char logout_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>body{text-align: center; background-color: grey;}</style>
</head>
<body>
  <p>You have beed logged out</p><a href="/">return to homepage</a>
  <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
</body>
</html>
)rawliteral";