start "Auto Test Good" python threaded_server.py /s
start "Status View" python status_viewer.py /s
SLEEP 30 
taskkill /f /FI "WINDOWTITLE eq Auto Test Good"
start "Auto Test Bad Json" python threaded_server_bad_json.py /s
start "Status View" python status_viewer.py /s
SLEEP 20
taskkill /f /FI "WINDOWTITLE eq Auto Test Bad Json"
