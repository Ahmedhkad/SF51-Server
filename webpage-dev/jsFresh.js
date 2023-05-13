var t = 10;
setInterval(function ( ) {
  document.getElementById("loading").style.visibility = "visible";
   document.getElementById("loading2").style.visibility = "visible";

  setTimeout(() => {  document.getElementById("loading").style.visibility = "hidden"; }, 500);
  setTimeout(() => {  document.getElementById("loading2").style.visibility = "hidden"; }, 500);
  // document.getElementById("loading").innerHTML = "loading ---";
  
    var xhttp = new XMLHttpRequest();
   xhttp.onreadystatechange = function() {
     if (this.readyState == 4 && this.status == 200) {
       var myObj = JSON.parse(this.responseText);
        console.log(myObj);
       
        var ssid = myObj.ssid;
        
        /* from 203 blue to 120 green 0 red */
 t = t + 1 ;
function createRemap(inMin, inMax, outMin, outMax) {
    return function remaper(x) {
        return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    };
} 
// var NewValuex = createRemap(0,60,220,0); 

var NewValue = (t - 0) * (0 - 220) / (60 - 0) + 220; 
document.getElementById("servertemp").style.backgroundColor  = 'hsl(' + [NewValue, '100%', '50%'] + ')';
// var t = 10;
        console.log(ssid);
        if (myObj.status == 0) {
            var statusx = "OFFLINE";
          }else if (myObj.status == 1){
         var statusx = "ONLINE";
          } 
          console.log(status);
        document.getElementById("statusID").innerHTML = statusx;
        document.getElementById("ssidID").innerHTML = ssid;
        document.getElementById("temperaturec").textContent =t.toString();
     }
   };
   xhttp.open("GET", "/readings", true);
   xhttp.send();
   
   }, 4000 ) ;

   function logoutButton() {
    document.getElementById("out").onclick = function () {
      location.href = "/logged-out";
  };
     
  }