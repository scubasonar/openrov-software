/*
 *
 * Description:
 * This script creates a directory and sends that as an argument to a spawned process (capture.cpp).
 * Then, it sends a request to capture a frame with file name of current time at a given interval.
 * Lastly, when (capture.cpp) responds with the file name (meaning save completed), it reads the file
 * and then emits the content to the Node.js server in base64 (string) format.
 *
 */

var spawn = require('child_process').spawn
  , util = require('util')
  , request = require('request')
  , EventEmitter = require('events').EventEmitter
  , fs = require('fs')
  , path = require('path')
  , CONFIG = require('./config')
  , logger = require('./logger').create(CONFIG)
  , orutils = require('./orutils')
  , moment = require('moment')
  ;

var OpenROVCamera = function (options) {
  var camera = new EventEmitter();
  var capture_process;

  // Open mjpg_streamer app as a child process
  var cmd = 'mjpg_streamer';  // rename to correspond with your C++ compilation
  var default_opts = {
    device : CONFIG.video_device,
    resolution : CONFIG.video_resolution,
    framerate : CONFIG.video_frame_rate,
    port : CONFIG.video_port
  };

  var options = orutils.mixin(options, default_opts);
  var _capturing = false;
  var args= [ '-i' ,
              '/usr/local/lib/input_uvc.so -r ' + options.resolution + ' -f ' + options.framerate,
              '-o',
              '/usr/local/lib/output_http.so -p ' + options.port
            ];
            
  // End camera process gracefully
  camera.close = function() {
    if (!_capturing) return;
    logger.log('closing camera on', options.device);
    _capturing = false;
    logger.log('sending SIGHUP to capture process');
    process.kill(capture_process.pid, 'SIGHUP');
  }

  camera.snapshot = function(callback) {
    if (!_capturing) return;
    var filename = CONFIG.preferences.get('photoDirectory') + '/ROV'+ moment().format("YYYYMMDDHHmmss") +'.jpg';
    request('http://localhost:' + options.port +'/?action=snapshot').pipe(fs.createWriteStream(filename));
    callback(filename);
  }
  
  // Actual camera capture starting mjpg-stremer
   
  camera.capture = function (callback) {
    logger.log('initiating camera on', options.device);

    // if camera working, should be at options.device (most likely /dev/video0 or similar)
    fs.exists(options.device, function(exists) {
      // no camera?!
      if (!exists) return callback(new Error(options.device + ' does not exist'));
      // wooooo!  camera!
      logger.log(options.device, ' found');
      _capturing = true; // then remember that we're capturing
      logger.log('spawning capture process...');

      capture_process = spawn(cmd, args);
      camera.emit('started');
      console.log('camera started');
      
      capture_process.stdout.on('data', function (data) {
        console.log('stdout: ' + data);
      });
      
      capture_process.stderr.on('data', function (data) {
        console.log('stderr: ' + data);
      });
      
      capture_process.on('exit', function (code) {
        console.log('child process exited with code ' + code);
      });      
    });
  };
  return camera;
};

module.exports = OpenROVCamera;
