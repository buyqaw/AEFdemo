function bytesToString(buffer) {
    return String.fromCharCode.apply(null, new Uint8Array(buffer));
}


var dataS = '';

function toUTF8Array(str) {
    var utf8 = [];
    for (var i=0; i < str.length; i++) {
        var charcode = str.charCodeAt(i);
        if (charcode < 0x80) utf8.push(charcode);
        else if (charcode < 0x800) {
            utf8.push(0xc0 | (charcode >> 6),
                      0x80 | (charcode & 0x3f));
        }
        else if (charcode < 0xd800 || charcode >= 0xe000) {
            utf8.push(0xe0 | (charcode >> 12),
                      0x80 | ((charcode>>6) & 0x3f),
                      0x80 | (charcode & 0x3f));
        }
        // surrogate pair
        else {
            i++;
            // UTF-16 encodes 0x10000-0x10FFFF by
            // subtracting 0x10000 and splitting the
            // 20 bits of 0x0-0xFFFFF into two halves
            charcode = 0x10000 + (((charcode & 0x3ff)<<10)
                      | (str.charCodeAt(i) & 0x3ff))
            utf8.push(0xf0 | (charcode >>18),
                      0x80 | ((charcode>>12) & 0x3f),
                      0x80 | ((charcode>>6) & 0x3f),
                      0x80 | (charcode & 0x3f));
        }
    }
    return utf8;
}


function synch(){
    var socket = new Socket();
        socket.open(
          "192.168.1.68",
          3737,
          function() {
            console.log("Connected");
            var dataString = dataS;

            var data = new toUTF8Array(dataString)
            socket.write(data);
            console.log("Socket sent");
            socket.shutdownWrite();
          },
          function(errorMessage) {
            alert("Подключитесь к интернету");
          	location.href='index.html';
            console.log("No connection");
          });
}


function stateChange(tlg) {
  document.getElementById("prohod").innerHTML="Открыто";
  var date = Math.round(+new Date()/1000);
  var message = localStorage.getItem("username") + "=" + localStorage.getItem("work") + "=" + localStorage.getItem("company") + "=" + localStorage.getItem("phone");
  dataS = message;
  synch();
  console.log(message);
    setTimeout(function () {
        if (1 == 1) {
            document.getElementById("prohod").innerHTML="Закрыто";
        }
    }, 5000);
}


// only works for ASCII characters
function stringToBytes(string) {
    var array = new Uint8Array(string.length);
    for (var i = 0, l = string.length; i < l; i++) {
        array[i] = string.charCodeAt(i);
    }
    return array.buffer;
}


// Nordic UART Service
var SERVICE_UUID = 'BA10';
var TX_UUIDs = "BA52";
var update = []


var app = {
    // Application Constructor
    initialize: function() {
        this.bindEvents();
    },
    bindEvents: function() {
        document.addEventListener('deviceready', this.onDeviceReady, false);
    },
    onDeviceReady: function() {
      if (localStorage.getItem("username") != null) {
        var property = blePeripheral.properties;
        var permission = blePeripheral.permissions;

        blePeripheral.onWriteRequest(app.didReceiveWriteRequest);
        blePeripheral.onBluetoothStateChange(app.onBluetoothStateChange);
        // 2 different ways to create the service: API calls or JSON
        app.createService();
      };
    },
    createService: function() {
        // https://learn.adafruit.com/introducing-the-adafruit-bluefruit-le-uart-friend/uart-service
        // Characteristic names are assigned from the point of view of the Central device

        var property = blePeripheral.properties;
        var permission = blePeripheral.permissions;
        Promise.all([
            blePeripheral.createService(SERVICE_UUID),
            blePeripheral.addCharacteristic(SERVICE_UUID, TX_UUID, property.WRITE, permission.WRITEABLE, property.READ | property.NOTIFY, permission.READABLE),
            blePeripheral.publishService(SERVICE_UUID),
            blePeripheral.startAdvertising(SERVICE_UUID)
        ]).then(
            function() { console.log ('Created UART Service'); },
            app.onError
        );

        blePeripheral.onWriteRequest(app.didReceiveWriteRequest);
    },

    didReceiveWriteRequest: function(request) {
        var message = bytesToString(request.value);
        console.log(message);
        if(message!="0"){
          stateChange(message);
        }
    },
    onBluetoothStateChange: function(state) {
        console.log('Bluetooth State is', state);
        if(state == "off"){
          sosto.innerHTML='ВЫКЛ.';
        }
        else{
          sosto.innerHTML='ВКЛ.';
        }

    }
};

app.initialize();
