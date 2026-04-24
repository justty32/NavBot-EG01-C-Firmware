#ifndef WEB_RESOURCES_H
#define WEB_RESOURCES_H

// 使用 R"rawliteral(...)rawliteral" 語法直接嵌入 HTML
// 這樣就不需要 LittleFS 了

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>NavBot EG01 Control</title>
    <style>
        body { font-family: sans-serif; text-align: center; background: #222; color: #fff; touch-action: manipulation; }
        .container { display: flex; flex-direction: column; align-items: center; padding: 20px; }
        .slider-group { width: 90%; margin: 15px 0; }
        input[type=range] { width: 100%; height: 30px; }
        .btn-group { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; width: 90%; }
        button { height: 50px; font-size: 18px; background: #444; color: #fff; border: 1px solid #666; border-radius: 8px; }
        button:active { background: #666; }
        .active { background: #007bff !important; }
        h2 { margin: 10px 0; color: #007bff; }
    </style>
</head>
<body>
    <div class="container">
        <h2>NavBot EG01</h2>
        
        <div class="slider-group">
            <label>速度 (Speed): <span id="spd_val">0</span></label>
            <input type="range" min="-0.06" max="0.06" step="0.005" value="0" oninput="send('/spd=' + this.value); document.getElementById('spd_val').innerHTML=this.value;">
        </div>

        <div class="slider-group">
            <label>左轉/右轉 (L/R): <span id="lr_val">1.0</span></label>
            <input type="range" min="0.5" max="1.5" step="0.05" value="1.0" oninput="send('/L=' + this.value + '&R=' + (2-this.value)); document.getElementById('lr_val').innerHTML=this.value;">
        </div>

        <div class="slider-group">
            <label>高度 (Height): <span id="h_val">100</span></label>
            <input type="range" min="60" max="120" step="1" value="100" oninput="send('/H=' + this.value); document.getElementById('h_val').innerHTML=this.value;">
        </div>

        <div class="btn-group">
            <button id="btn_trot" class="active" onclick="setMode(0)">Trot 步態</button>
            <button id="btn_walk" onclick="setMode(1)">Walk 步態</button>
            <button id="btn_stab" onclick="toggleStab()">平衡: OFF</button>
            <button onclick="window.location.href='/cal'">校準頁面</button>
        </div>
    </div>

    <script>
        function send(path) {
            fetch(path).catch(() => {});
        }
        function setMode(m) {
            send('/mode=' + m);
            document.getElementById('btn_trot').className = (m==0?'active':'');
            document.getElementById('btn_walk').className = (m==1?'active':'');
        }
        let stab = false;
        function toggleStab() {
            stab = !stab;
            send('/stab=' + (stab?1:0));
            document.getElementById('btn_stab').innerHTML = '平衡: ' + (stab?'ON':'OFF');
            document.getElementById('btn_stab').className = (stab?'active':'');
        }
    </script>
</body>
</html>
)rawliteral";

const char CAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>NavBot Calibration</title>
    <style>
        body { font-family: sans-serif; background: #f0f0f0; padding: 20px; }
        .card { background: #fff; padding: 20px; border-radius: 8px; max-width: 500px; margin: auto; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .row { display: flex; justify-content: space-between; margin: 10px 0; align-items: center; }
        input { width: 60px; text-align: center; }
        button { width: 100%; height: 40px; margin-top: 20px; background: #28a745; color: #fff; border: none; border-radius: 4px; cursor: pointer; }
    </style>
</head>
<body>
    <div class="card">
        <h3>舵機中點與幾何校準</h3>
        <form action="/save_cal" method="GET">
            <div class="row">腿1 Thigh/Shank: <input name="h1" value="90"> <input name="s1" value="90"></div>
            <div class="row">腿2 Thigh/Shank: <input name="h2" value="90"> <input name="s2" value="90"></div>
            <div class="row">腿3 Thigh/Shank: <input name="h3" value="90"> <input name="s3" value="90"></div>
            <div class="row">腿4 Thigh/Shank: <input name="h4" value="90"> <input name="s4" value="90"></div>
            <hr>
            <div class="row">機械構型 (0:Series, 1:Parallel): <input name="ma" value="0"></div>
            <div class="row">初始 Pitch/Roll: <input name="ip" value="0"> <input name="ir" value="0"></div>
            <button type="submit">儲存並應用</button>
            <button type="button" style="background:#dc3545;" onclick="location.href='/'">回控制頁面</button>
        </form>
    </div>
</body>
</html>
)rawliteral";

#endif
