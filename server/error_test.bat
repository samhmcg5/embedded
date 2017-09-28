start "Auto Test Good" C:\\Users\Eric\AppData\Local\Programs\Python\Python36-32\python.exe threaded_server.py /s
SLEEP 20 
taskkill /f /FI "WINDOWTITLE eq Auto Test Good"
start "Auto Test Bad Json" C:\\Users\Eric\AppData\Local\Programs\Python\Python36-32\python.exe threaded_server_bad_json.py /s
SLEEP 10
taskkill /f /FI "WINDOWTITLE eq Auto Test Bad Json"
start "Auto Test Bad Sequence" C:\\Users\Eric\AppData\Local\Programs\Python\Python36-32\python.exe threaded_server_bad_sequence.py /s
SLEEP 10 
taskkill /f /FI "WINDOWTITLE eq Auto Test Bad Sequence"
exit