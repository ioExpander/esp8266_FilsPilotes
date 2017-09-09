const PING_INTERVAL_MILLIS = 5000;

var output = document.getElementById('output');
var connectionLabel = document.getElementById('connectionLabel');
var socket;
var EspHostname = '192.168.1.31';
//var EspHostname = 'actarus.local';

$(document).ready(function() {
  changeHostName(EspHostname);
  //$("#status").text("No Status");
  getEspStatus();
  generateFPCommandsList();
});

function changeHostName(newHost) {
  EspHostname = newHost;
  $('#espHostname').attr('value',EspHostname);
  $('#wsUrl').attr('value', 'ws://'+EspHostname+':81/debug');
}

function getEspStatus() {
  var $statusList = $('#espStatusList');
  $.ajax({
    type: 'GET',
    url: 'http://'+EspHostname+'/api/espStatus',
    success: function(espStatus) {
      $statusList.empty();
      $.each(espStatus, function(espStatusKey, espStatusValue) {
        //console.log(espStatusValue, espStatusKey);
        $statusList.append('<li>'+espStatusKey+': '+espStatusValue+'</li>');
      });
    },
    error: function () {
      console.log("Error Fetching Esp Status Data");
    }
  });
}

function generateFPCommandsList() {

  $.getJSON('http://'+EspHostname+'/api/fpStatus', function(data) {
    var fpStatusString = "CCCCAC";
    fpStatusString = (data["FpValues"]);
    for (let i=0; i< fpStatusString.length; i++) {
      let $fpButton = $('#btn_fp'+(i+1)+fpStatusString[i]);
      $fpButton.prop('disabled', true);
    }
    //console.log(FpStatusString);
  });

  var isFpCommandsDebugModeOn = $('#isFpCommandsDebugModeOn').is(':checked');
  var $fpCommandsList = $('#fpCommandsList');
  $fpCommandsList.empty();
  var cmdList = ['C', 'E', 'A', 'H'];

  for (let i=0; i< 6; i++) {
    let fpDisplayID = i+1;
    let $newListItem = $('<li>', {
      html : 'FP'+fpDisplayID + ': '
    });
    for (let tempCmd of cmdList) {
      if (isFpCommandsDebugModeOn) {
        let url = 'http://'+ EspHostname +'/api/setfp?fpId='+fpDisplayID+'&cmd='+tempCmd;

        let $newLink = $('<a>', {
          href : url,
          html : tempCmd
        }).appendTo($newListItem).clone();
      $newListItem.append($("<span>&nbsp;&nbsp;</span>"));
      } else {
        let $newButton = $('<input>', {
          type: "button",
          id: 'btn_fp'+fpDisplayID+tempCmd,
          value: tempCmd,
          onClick: 'callSetFp('+fpDisplayID+',"'+tempCmd+'");'
        }).appendTo($newListItem).clone();
      }
    $newListItem.append($("<span>&nbsp;&nbsp;&nbsp;</span>"));
    }

    $fpCommandsList.append($newListItem.clone());


  }

}

function callSetFp(fpDisplayID, cmdToSend) {
  $.ajax({
    type: 'GET',
    url: 'http://'+ EspHostname +'/api/setfp?fpId='+fpDisplayID+'&cmd='+cmdToSend,
    success: function(setFpResponse) {
      $.each(setFpResponse, function(setFprResponseKey, setFpResponseValue) {
        //console.log(setFprResponseKey, setFpResponseValue);
      });
    },
    error: function () {
      console.log("Error setFP Data");
    }
  });
  generateFPCommandsList();
}

function getTeleInfoStatus() {
  var $statusList = $('#teleInfoStatusList');
  $.ajax({
    type: 'GET',
    url: 'http://'+EspHostname+'/api/teleInfoStatus',
    success: function(teleInfoStatus) {
      $statusList.empty();
      $.each(teleInfoStatus, function(teleInfoStatusKey, teleInfoStatusValue) {
        $statusList.append('<li>'+teleInfoStatusKey+': '+teleInfoStatusValue+'</li>');
      });
    },
    error: function () {
      console.log("Error Fetching TeleInfo Status Data");
    }
  });
}

function fetchEspCrashInfo() {
  $.ajax({
    type: 'GET',
    url: 'http://'+EspHostname+'/api/getCrashInfo',
    success: function(espCrash) {
      console.log("Got Crashes Info");
      console.log(espCrash);
      $('#espCrashReport').text(espCrash);
    },
    error: function () {
      console.log("Error Fetching Crash Info");
    }
  });
}

function resetCrashInfo() {
  $.ajax({
    type: 'GET',
    url: 'http://'+EspHostname+'/api/clearCrashes',
    success: function(espCrash) {
      console.log("Crashes Cleared");
    },
    error: function () {
      console.log("Error Clearing Crashes");
    }
  });
}


function generateEspCrash(crashType) {
  $.ajax({
    type: 'GET',
    url: 'http://'+EspHostname+'/api/generateCrash?crashType='+crashType,
    success: function(espCrash) {
      console.log("Crashed");
    },
    error: function () {
      console.log("Error Generating Crash");
    }
  });
}


function disconnectWsDebug(host) {
  if(socket) {
    socket.close();
    socket = null;
  }
}

function connectWsDebug(host) {
  console.log('connect', host);
  if (window.WebSocket) {
    connectionLabel.value = "Connecting";
    if(socket) {
      socket.close();
      socket = null;
    }

    socket = new WebSocket(host);

    socket.onmessage = function(event) {
      /*var date = new Date();
      output.value += "[";
      output.value += (('0' + date.getHours()).slice(-2));
      output.value += ":";
      output.value += (('0' + date.getMinutes()).slice(-2));
      output.value += ":";
      output.value += (('0' + date.getSeconds()).slice(-2));
      output.value += "]";
      output.value += " ";
      */
      output.value += event.data; // + "\r\n";
      var textarea = document.getElementById('output');
      textarea.scrollTop = textarea.scrollHeight;
    };
    socket.onopen = function(event) {
      isRunning = true;
      connectionLabel.value = "Connected";
    };
    socket.onclose = function(event) {
       isRunning = false;
       connectionLabel.value = "Disconnected";
       //socket.removeAllListeners();
       //socket = null;
    };
    socket.onerror = function(event) {
       connectionLabel.value = "Error";
       //	         socket.removeAllListeners();
       //	            socket = null;
    };
  } else {
      alert("Your browser does not support Web Socket.");
  }
}

function clearDebugText() {
    output.value="";
}
