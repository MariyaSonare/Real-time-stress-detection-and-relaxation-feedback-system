#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

#define EMG_PIN 34
#define BUZZER_PIN 25
#define DHT_PIN 4
#define DHT_TYPE DHT11

const char* AP_SSID = "Stress_EMG_ESP32";
const char* AP_PASS = "12345678";

WebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

static const int ADC_MAX = 4095;
static const float VREF = 3.3f;
static const uint32_t SAMPLE_PERIOD_MS = 5;

float baseline = 0.0f;
float rectified = 0.0f;
float rms = 0.0f;
float stressScore = 0.0f;
int stressClass = 0;

float BASELINE_ALPHA = 0.01f;
float RMS_ALPHA = 0.08f;
float SCORE_ALPHA = 0.12f;

float TH_RELAXED = 0.08f;
float TH_MODERATE = 0.18f;

float tempC = NAN, hum = NAN;
uint32_t lastDhtMs = 0;

uint32_t lastBeepMs = 0;

float adcToVolts(int adc){ return (adc * VREF) / (float)ADC_MAX; }

void updateDHT(){
  if(millis() - lastDhtMs < 2000) return;
  lastDhtMs = millis();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if(!isnan(h) && !isnan(t)){ hum = h; tempC = t; }
}

void classifyStress(){
  float adj = 0.0f;
  if(!isnan(tempC)) adj += (tempC - 28.0f) * 0.0025f;
  if(!isnan(hum))   adj += (hum - 55.0f)   * 0.0008f;

  float th1 = max(0.03f, TH_RELAXED + adj);
  float th2 = max(th1 + 0.03f, TH_MODERATE + adj);

  int cls = (rms < th1) ? 0 : (rms < th2) ? 1 : 2;

  float raw = 0.0f;
  if(rms <= th1) raw = (rms / th1) * 30.0f;
  else if(rms <= th2) raw = 30.0f + ((rms - th1) / (th2 - th1)) * 40.0f;
  else raw = 70.0f + min(30.0f, (rms - th2) * 120.0f);

  stressScore = (1.0f - SCORE_ALPHA) * stressScore + SCORE_ALPHA * raw;

  if(cls > stressClass) stressClass = cls;
  else if(cls < stressClass){
    if(stressClass == 2 && stressScore < 62) stressClass = 1;
    else if(stressClass == 1 && stressScore < 25) stressClass = 0;
  }
}

void relaxationFeedback(){
  uint32_t now = millis();
  if(stressClass == 2){
    if(now - lastBeepMs > 1100){
      lastBeepMs = now;
      digitalWrite(BUZZER_PIN, HIGH); delay(70);
      digitalWrite(BUZZER_PIN, LOW);  delay(60);
      digitalWrite(BUZZER_PIN, HIGH); delay(70);
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else if(stressClass == 1){
    if(now - lastBeepMs > 2600){
      lastBeepMs = now;
      digitalWrite(BUZZER_PIN, HIGH); delay(80);
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

const char PAGE_INDEX[] PROGMEM = R"HTML(
<!doctype html><html><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<title>Stress EMG</title>
<style>
:root{
  --bg:#070812; --card:rgba(255,255,255,.06); --br:rgba(130,170,255,.14);
  --txt:#ECF2FF; --mut:rgba(236,242,255,.72);
  --blue:#60A5FA; --g:#22C55E; --y:#FBBF24; --r:#FB7185;
  --shadow: 0 18px 60px rgba(0,0,0,.45);
}
*{box-sizing:border-box}
body{
  margin:0; font-family: ui-sans-serif,system-ui,-apple-system,Segoe UI,Roboto,Arial;
  color:var(--txt);
  background:
    radial-gradient(900px 600px at 15% 0%, rgba(96,165,250,.22), transparent 60%),
    radial-gradient(900px 600px at 85% 10%, rgba(251,113,133,.16), transparent 55%),
    radial-gradient(800px 550px at 40% 90%, rgba(34,197,94,.10), transparent 55%),
    linear-gradient(180deg, #060714 0%, #070812 50%, #050611 100%);
}
.wrap{max-width:1100px;margin:0 auto;padding:18px 16px 28px}
.header{display:flex;justify-content:space-between;align-items:center;gap:12px;margin-bottom:14px}
.brand{display:flex;flex-direction:column;gap:4px}
.h1{font-weight:950;letter-spacing:.2px;font-size:16px}
.sub{font-size:12px;color:var(--mut);line-height:1.35}
.chip{
  display:inline-flex;align-items:center;gap:8px;padding:8px 12px;border-radius:999px;
  border:1px solid var(--br); background: rgba(10,12,22,.55);
}
.dot{width:8px;height:8px;border-radius:999px;background:var(--g);box-shadow:0 0 0 6px rgba(34,197,94,.12)}
.grid{display:grid;grid-template-columns:1fr;gap:14px}
@media(min-width:980px){.grid{grid-template-columns: 420px 1fr}}
.card{
  background: linear-gradient(180deg, rgba(255,255,255,.08), rgba(255,255,255,.04));
  border:1px solid var(--br); border-radius:20px; padding:16px; box-shadow:var(--shadow);
  backdrop-filter: blur(10px);
}
.kpiTop{display:flex;gap:12px;align-items:center;justify-content:space-between}
.badge{
  display:inline-flex;align-items:center;gap:10px;padding:10px 12px;border-radius:14px;
  border:1px solid var(--br); background: rgba(10,12,22,.35);
  font-weight:900; letter-spacing:.2px;
}
.badge b{font-size:14px}
.small{font-size:12px;color:var(--mut)}
.ringWrap{display:flex;gap:14px;align-items:center;margin-top:12px}
.ring{width:120px;height:120px;position:relative}
.ring svg{transform:rotate(-90deg)}
.ring .center{
  position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;
  gap:2px;
}
.score{font-size:28px;font-weight:950}
.score small{font-size:12px;color:var(--mut);font-weight:800}
.metaRow{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin-top:12px}
.mini{
  border:1px solid var(--br); border-radius:16px; padding:12px;
  background: rgba(10,12,22,.32);
}
.mini .v{font-size:18px;font-weight:950;margin-top:6px}
.ctaRow{display:flex;gap:10px;flex-wrap:wrap;margin-top:12px}
.btn{
  appearance:none; border:1px solid rgba(130,170,255,.18); background: rgba(96,165,250,.10);
  color:var(--txt); padding:10px 12px; border-radius:14px; font-weight:950; cursor:pointer;
}
.btn:active{transform:scale(.99)}
.toggle{
  display:flex;align-items:center;gap:10px;border:1px solid rgba(130,170,255,.18);
  padding:10px 12px;border-radius:14px;background:rgba(10,12,22,.32); user-select:none;
  font-weight:900;
}
.switch{width:44px;height:26px;border-radius:999px;border:1px solid rgba(130,170,255,.20);background:rgba(255,255,255,.08);position:relative}
.knob{width:22px;height:22px;border-radius:999px;background:#fff;position:absolute;top:1px;left:1px;transition:all .18s ease}
.switch.on{background:rgba(34,197,94,.16);border-color:rgba(34,197,94,.25)}
.switch.on .knob{left:21px;background:rgba(34,197,94,1)}
.coach{
  margin-top:12px; padding:12px; border-radius:16px; border:1px solid var(--br);
  background: rgba(10,12,22,.32); line-height:1.35
}
.coach b{color:var(--txt)}
.right{display:flex;flex-direction:column;gap:14px}
.panelTitle{display:flex;justify-content:space-between;align-items:baseline}
.panelTitle .t{font-weight:950;letter-spacing:.3px}
.panelTitle .s{font-size:12px;color:var(--mut)}
canvas{
  width:100%; height:230px; border-radius:18px;
  border:1px solid rgba(130,170,255,.10);
  background: rgba(8,10,18,.55);
}
</style>
</head><body>
<div class="wrap">
  <div class="header">
    <div class="brand">
      <div class="h1">Real-time Stress Detection & Relaxation Feedback</div>
      <div class="sub">Connect to <b>Stress_EMG_ESP32</b> → open <b>192.168.4.1</b>. EMG + features + 3-level prediction + TTS.</div>
    </div>
    <div class="chip"><span class="dot"></span><span style="font-weight:950">Live</span></div>
  </div>

  <div class="grid">
    <div class="card">
      <div class="kpiTop">
        <div class="badge"><span id="tagIcon">●</span> <b id="type">—</b></div>
        <div class="small">3 Types: Relaxed • Moderate • High</div>
      </div>

      <div class="ringWrap">
        <div class="ring">
          <svg width="120" height="120">
            <circle cx="60" cy="60" r="50" stroke="rgba(255,255,255,.10)" stroke-width="12" fill="none"></circle>
            <circle id="arc" cx="60" cy="60" r="50" stroke="rgba(34,197,94,1)" stroke-width="12" stroke-linecap="round" fill="none"
              stroke-dasharray="314" stroke-dashoffset="314"></circle>
          </svg>
          <div class="center">
            <div class="score"><span id="score">0</span><small>/100</small></div>
            <div class="small">Stress Score</div>
          </div>
        </div>

        <div style="flex:1;display:flex;flex-direction:column;gap:10px">
          <div class="mini">
            <div class="small">Current EMG Feature (RMS)</div>
            <div class="v"><span id="rms">0.000</span> <span class="small">V</span></div>
          </div>
          <div class="mini">
            <div class="small">Breathing Cue</div>
            <div class="v" id="cue">Standby</div>
          </div>
        </div>
      </div>

      <div class="metaRow">
        <div class="mini">
          <div class="small">Temperature</div>
          <div class="v"><span id="temp">—</span> <span class="small">°C</span></div>
        </div>
        <div class="mini">
          <div class="small">Humidity</div>
          <div class="v"><span id="hum">—</span> <span class="small">%</span></div>
        </div>
      </div>

      <div class="ctaRow">
        <button class="btn" onclick="speak()">🔊 Speak</button>
        <div class="toggle" onclick="toggleAuto()">
          <div>Auto-speak on High</div>
          <div id="sw" class="switch"><div class="knob"></div></div>
        </div>
      </div>

      <div class="coach" id="coach">
        <b>Coach:</b> Standby. Keep shoulders relaxed. Slow the exhale.
      </div>
    </div>

    <div class="right">
      <div class="card">
        <div class="panelTitle">
          <div class="t">EMG Signal</div>
          <div class="s">Rectified volts • smooth trace</div>
        </div>
        <div style="height:10px"></div>
        <canvas id="c1" width="900" height="230"></canvas>
      </div>

      <div class="card">
        <div class="panelTitle">
          <div class="t">Prediction Trend</div>
          <div class="s">RMS + Score overlay</div>
        </div>
        <div style="height:10px"></div>
        <canvas id="c2" width="900" height="230"></canvas>
      </div>
    </div>
  </div>
</div>

<script>
const N=240;
const emg=Array(N).fill(0), rms=Array(N).fill(0), score=Array(N).fill(0);
const c1=document.getElementById('c1').getContext('2d');
const c2=document.getElementById('c2').getContext('2d');

let autoSpeak=false, lastSpokenAt=0, lastCls=-1;

function pal(cls){
  const cs=getComputedStyle(document.documentElement);
  if(cls===0) return {name:"Relaxed", col:cs.getPropertyValue('--g').trim(), icon:"●"};
  if(cls===1) return {name:"Moderate", col:cs.getPropertyValue('--y').trim(), icon:"●"};
  return {name:"High", col:cs.getPropertyValue('--r').trim(), icon:"●"};
}

function setCoach(cls){
  const coach=document.getElementById('coach');
  const cue=document.getElementById('cue');
  if(cls===2){
    cue.innerText="4-4-6";
    coach.innerHTML="<b>Coach:</b> High stress. Do <b>4-4-6</b> breathing (In 4s • Hold 4s • Out 6s). Unclench jaw.";
  } else if(cls===1){
    cue.innerText="Slow Exhale";
    coach.innerHTML="<b>Coach:</b> Moderate. Extend exhale (In 4s • Out 6s). Drop shoulders.";
  } else {
    cue.innerText="Standby";
    coach.innerHTML="<b>Coach:</b> Relaxed. Keep steady breathing. Maintain soft posture.";
  }
}

function setRing(scoreVal, color){
  const arc=document.getElementById('arc');
  const circum=314;
  const s=Math.max(0,Math.min(100,scoreVal));
  const off=circum*(1-(s/100));
  arc.style.stroke=color;
  arc.setAttribute("stroke-dashoffset", off.toFixed(2));
}

function speak(){
  const t=document.getElementById('type').innerText;
  const s=document.getElementById('score').innerText;
  const msg=`${t}. Stress score ${s} out of 100. Follow the breathing coach.`;
  if('speechSynthesis' in window){
    speechSynthesis.cancel();
    const u=new SpeechSynthesisUtterance(msg);
    u.rate=1.0; u.pitch=1.0;
    speechSynthesis.speak(u);
  }
}

function toggleAuto(){
  autoSpeak=!autoSpeak;
  const sw=document.getElementById('sw');
  sw.className = autoSpeak ? "switch on" : "switch";
}

function grid(ctx){
  const w=ctx.canvas.width,h=ctx.canvas.height;
  ctx.clearRect(0,0,w,h);
  ctx.strokeStyle='rgba(220,235,255,0.06)';
  ctx.lineWidth=1;
  for(let i=1;i<8;i++){
    const y=h*i/8;
    ctx.beginPath(); ctx.moveTo(0,y); ctx.lineTo(w,y); ctx.stroke();
  }
}

function plot(ctx,arr,color,min,max,fill=false){
  const w=ctx.canvas.width,h=ctx.canvas.height;
  ctx.lineWidth=2;
  ctx.strokeStyle=color;
  ctx.beginPath();
  for(let i=0;i<arr.length;i++){
    let v=arr[i]; if(v<min) v=min; if(v>max) v=max;
    const x=w*i/(arr.length-1);
    const y=h-(v-min)/(max-min)*h;
    if(i===0) ctx.moveTo(x,y); else ctx.lineTo(x,y);
  }
  ctx.stroke();

  if(fill){
    ctx.lineTo(w,h); ctx.lineTo(0,h); ctx.closePath();
    ctx.fillStyle=color.replace("1)", "0.12)").replace("0.95", "0.12");
    ctx.fill();
  }
}

async function tick(){
  try{
    const r=await fetch('/api/data');
    const j=await r.json();

    const p=pal(j.cls);
    document.getElementById('type').innerText=p.name;
    document.getElementById('tagIcon').innerText=p.icon;
    document.getElementById('score').innerText=Math.round(j.score);
    document.getElementById('rms').innerText=(j.rmsV??0).toFixed(3);
    document.getElementById('temp').innerText=(j.temp??0).toFixed(1);
    document.getElementById('hum').innerText=(j.hum??0).toFixed(1);

    setCoach(j.cls);
    setRing(j.score, p.col);

    emg.push(j.emgV); emg.shift();
    rms.push(j.rmsV); rms.shift();
    score.push(j.score); score.shift();

    grid(c1);
    plot(c1, emg, 'rgba(96,165,250,0.95)', 0.0, 0.6, true);

    grid(c2);
    plot(c2, rms, 'rgba(34,197,94,0.92)', 0.0, 0.35, true);
    const sScaled = score.map(x => (x/100)*0.35);
    plot(c2, sScaled, 'rgba(251,113,133,0.92)', 0.0, 0.35, false);

    const now=Date.now();
    if(autoSpeak && j.cls===2 && (now-lastSpokenAt>6000) && (lastCls!==2)){
      lastSpokenAt=now; lastCls=2; speak();
    }
    if(j.cls!==2) lastCls=j.cls;

  }catch(e){}
}
setInterval(tick, 200);
</script>
</body></html>
)HTML";

void handleRoot(){ server.send_P(200, "text/html", PAGE_INDEX); }

void handleData(){
  String json="{";
  json += "\"emgV\":" + String(rectified,4) + ",";
  json += "\"rmsV\":" + String(rms,4) + ",";
  json += "\"score\":" + String(stressScore,2) + ",";
  json += "\"cls\":" + String(stressClass) + ",";
  json += "\"temp\":" + String(isnan(tempC)?0.0f:tempC,1) + ",";
  json += "\"hum\":"  + String(isnan(hum)?0.0f:hum,1);
  json += "}";
  server.send(200,"application/json",json);
}

uint32_t lastSampleMs = 0;

void setup(){
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  dht.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/api/data", handleData);
  server.begin();

  float sum=0;
  for(int i=0;i<200;i++){ sum += adcToVolts(analogRead(EMG_PIN)); delay(2); }
  baseline = sum/200.0f;
}

void loop(){
  server.handleClient();
  updateDHT();

  uint32_t now = millis();
  if(now - lastSampleMs >= SAMPLE_PERIOD_MS){
    lastSampleMs = now;

    float v = adcToVolts(analogRead(EMG_PIN));
    baseline = (1.0f-BASELINE_ALPHA)*baseline + BASELINE_ALPHA*v;
    float hp = v - baseline;

    rectified = fabs(hp);

    float sq = rectified*rectified;
    float rmsSq = (1.0f-RMS_ALPHA)*(rms*rms) + RMS_ALPHA*sq;
    rms = sqrt(max(0.0f, rmsSq));

    classifyStress();
    relaxationFeedback();
  }
}
