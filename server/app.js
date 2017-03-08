// TODO authorization fails after a while (token expires), fix
const Mopidy = require('mopidy');
const SpotifyWebApi = require('spotify-web-api-node');

const mopidy = new Mopidy({
    webSocketUrl: "ws://umbreon.mit.edu:6680/mopidy/ws/"
});

const spotifyApi = new SpotifyWebApi({
  clientId : 'b2c6785eb2a9459983dca71989b54ed6',
  clientSecret : '71921683e898433cb8601487b2716112',
  redirectUri : 'http://www.google.com/'
});

const express = require('express');
const app = express();

const BEAT_CONFIDENCE_THRESHOLD = 0.0;

app.get('/track', async function (req, res) {
  await getspotifycredentials();

  let currenttrack = await mopidy.playback.getcurrenttrack();
  res.send(currenttrack);
});

app.get('/time', async function (req, res) {
  let time = await mopidy.playback.getTimePosition();
  res.send(time.toString());
});

app.get('/state', async function (req, res) {
  let state = await mopidy.playback.getState();
  res.send(state);
});

app.get('/tatums', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  console.log(currentTrack);
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let beats = trackAnalysis.tatums.filter(beat => beat.confidence > 0);
  res.send(beats);
});

app.get('/beatsraw', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  console.log(currentTrack);
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let beats = trackAnalysis.beats.filter(beat => beat.confidence > 0);
  res.send(beats);
});

app.get('/bars', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let bars = trackAnalysis.bars.map(bar => Math.floor(bar.start * 1000));
  res.send(bars);
});

app.get('/sections', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let sections = trackAnalysis.sections.map(section => Math.floor(section.start * 1000));
  res.send(sections);
});

app.get('/sectionsraw', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let beats = trackAnalysis.beats;
  res.send(trackAnalysis.sections);
});

app.get('/tempo', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let beats = trackAnalysis.beats;
  res.send(trackAnalysis.track);
});

app.get('/raw', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let beats = trackAnalysis.beats.filter(beat => beat.confidence > BEAT_CONFIDENCE_THRESHOLD)
                                 .map(beat => Math.floor(beat.start * 1000));
  res.send(trackAnalysis);
});

app.get('/beats', async function (req, res) {
  await getSpotifyCredentials();

  //let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    //currentTrack = await mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  //};

  //mopidy.on('event:trackPlaybackStarted', respondToChange);

  let currentTrack = await mopidy.playback.getCurrentTrack();
  let trackAnalysis = await getTrackAnalysis(currentTrack.uri.split(':').pop());
  let beats = trackAnalysis.beats.filter(beat => beat.confidence > BEAT_CONFIDENCE_THRESHOLD)
                                 .map(beat => Math.floor(beat.start * 1000));
  res.send(beats);
});

app.listen(3000, function () {
  console.log('Example app listening on port 3000!');
})

let getSpotifyCredentials = async function() {
  if (spotifyApi.getAccessToken()) return;

  try {
    let data = await spotifyApi.clientCredentialsGrant();  // response data
    console.log('The access token expires in ' + data.body['expires_in']);
    console.log('The access token is ' + data.body['access_token']);

    spotifyApi.setAccessToken(data.body['access_token']);
  } catch (err) {
    console.log('Something went wrong when retrieving an access token', err);
  }
}

let getTrackAnalysis = async function(track) {
  try {
    let analysis = await spotifyApi.getAudioAnalysisForTrack(track);
    return analysis.body;
  } catch (err) {
    console.log('error retrieving track analysis', err);
  }
}

let main = async function() {
  await getSpotifyCredentials();

  let respondToChange = async function() {
    // TODO maybe change this to use on(listener) and listen for generic event,
    // rather than calling on(event, listener) for each event?

    // fetch spotify data every time (FIXME)
    //awiat(
    // FIXME idk why this doesn't work with a 'spotify:track:blahblah' type URI, but it doesn't...
    //mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis({uri: track.uri}));
    mopidy.playback.getCurrentTrack().then(track => getTrackAnalysis(track.uri.split(':').pop()));
  };

  mopidy.on('event:trackPlaybackStarted', respondToChange);
}

//main();
