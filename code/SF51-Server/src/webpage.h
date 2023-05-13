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
    }

    .header {
    font-size: 16px;
    }

    .temp {
    color: black;
    font-size: 20px;
    margin: 1em auto;
    width: 40em;
    padding: 10px;
    border: 3px solid;
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
    font-size: 18px;
    padding: 6px 15px;
    margin-top: 5px;
    margin-right: 5px;
    border-radius: 5px;
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
    </div>
    <div class="header"> 
        <h1>SF51-Server</h1>
        <h3>Web Server Control</h3>
    </div>
    <p id="servertemp" class="temp"> Server Temperture: 
       <span id="temperaturec">--</span>
       <sup class="units">&deg;c</sup>
   </p>
    <div class="container">
        <div class="box">
            <p>LED State : <span id="StrongServerLEDid">---</span> </p>
            <p>Ping State : <span class="%StrongServerC%" id="StrongServerCid">%StrongServerC%</span> </p>

            <button class="button" onmousedown="toggleCheckbox('onStrongServer');"
                ontouchstart="toggleCheckbox('onStrongServer');" onmouseup="toggleCheckbox('offStrongServer');"
                ontouchend="toggleCheckbox('offStrongServer');">Start StrongServer</button>
        </div>

        <div class="box">
             <p>LED State : <span id="CoolServerLEDid">---</span> </p>
            <p>Ping State : <span class="%CoolServerC%" id="CoolServerCid">%CoolServerC%</span> </p>

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
    var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
       var myObj = JSON.parse(this.responseText);
        console.log(myObj);
        var Temperature = myObj.Temperature;
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
   }, 5000 ) ;

   setInterval(function ( ) {
   var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("StrongServerCid").innerHTML = this.responseText;
      document.getElementById("StrongServerCid").className = this.responseText;
    }
  };
  xhttp.open("GET", "/StrongPing", true);
  xhttp.send();
  }, 4000 ) ;

  setInterval(function ( ) {
   var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("CoolServerCid").innerHTML = this.responseText;
      document.getElementById("CoolServerCid").className = this.responseText;
    }
  };
  xhttp.open("GET", "/CoolPing", true);
  xhttp.send();
  }, 3000 ) ;

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
</head>
<body>
  <p>Logged out or <a href="/">return to homepage</a>.</p>
  <p><strong>Note:</strong> close all web browser tabs to complete the logout process.</p>
</body>
</html>
)rawliteral";