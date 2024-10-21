const char INDEX_HTML[] = R"rawliteral(
<html>
  <head>
    <title>CameraBrick</title>
    <meta name="viewport" content="width=device-width, height=device-height, initial-scale=1">
    <style>
      img {  
        display:block;
        object-fit: cover;
        max-width: devide-width;
        height: 100%; 
        margin: auto;
      }
    </style>
  </head>
  <body>
    <div>
      <img src="" id="stream">
    </div>
   <script>
   window.onload = document.getElementById("stream").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>
)rawliteral";