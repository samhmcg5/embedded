start "Auto Test Good" python threaded_server.py /s
SLEEP 20 
taskkill /f /FI "WINDOWTITLE eq Auto Test Good"
start "Auto Test Bad Json" python threaded_server_bad_json.py /s
SLEEP 10
taskkill /f /FI "WINDOWTITLE eq Auto Test Bad Json"
start "Auto Test Bad Sequence" python threaded_server_bad_sequence.py /s
SLEEP 10 
taskkill /f /FI "WINDOWTITLE eq Auto Test Bad Sequence"
exit
