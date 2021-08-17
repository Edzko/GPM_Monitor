REM
python GetPWT.py -p my-project-1516154946730 -k C:/users/edzko/rsa_private.pem

curl -X POST -H 'authorization: Bearer JWT' -H 'content-type: application/json' --data '{"binary_data": "DATA"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/{project-id}/locations/{cloud-region}/registries/{registry-id}/devices/{device-id}:publishEvent'


curl -X POST -H 'authorization: eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE2MjQ5MDA2OTYsImV4cCI6MTYyNDkwNDI5NiwiYXVkIjoibXktcHJvamVjdC0xNTE2MTU0OTQ2NzMwIn0.LFfTCWKBc9bIvMdTsq3u7jNUKnu_QKcKZvO6lJQ8mvcqURaB6vZYBV_5gdI8rJ7xbS5SUdo9vp26refwBNl9HY9eg0ASsjKxcuqZUzNisbLfw_dubol2ejzP3U0CRoMkAcQhomfue7FrZ4dmBFIgnXOmx9Hj5DX9tfxHqVjtquexdjCU1o2IJarLfVp17kgiqQCzyCTl_KqAgnopf0fimhCU1m7hAGGN6ou08YPrY3IOHVYaHaJ2rajSPQxjecLDEW7wLG3jQbD3FuFLHQZ4t9881H1_cqdKV_CNOWryFP4qtK8YNgTOLscCUkmh8_JhtBdy3SaaElr5gaiNSU0FzQ' -H 'content-type: application/json' --data '{"binary_data": "my first data", "VIB_DATA": "VIB_DATA"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/my-project-1516154946730/locations/global/registries/MAGNA_GPM_DEV_EDZKO/devices/GPM-1D.1:publishEvent' --libcurl iot.c


curl -H 'authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE2MjQ5MDA2OTYsImV4cCI6MTYyNDkwNDI5NiwiYXVkIjoibXktcHJvamVjdC0xNTE2MTU0OTQ2NzMwIn0.LFfTCWKBc9bIvMdTsq3u7jNUKnu_QKcKZvO6lJQ8mvcqURaB6vZYBV_5gdI8rJ7xbS5SUdo9vp26refwBNl9HY9eg0ASsjKxcuqZUzNisbLfw_dubol2ejzP3U0CRoMkAcQhomfue7FrZ4dmBFIgnXOmx9Hj5DX9tfxHqVjtquexdjCU1o2IJarLfVp17kgiqQCzyCTl_KqAgnopf0fimhCU1m7hAGGN6ou08YPrY3IOHVYaHaJ2rajSPQxjecLDEW7wLG3jQbD3FuFLHQZ4t9881H1_cqdKV_CNOWryFP4qtK8YNgTOLscCUkmh8_JhtBdy3SaaElr5gaiNSU0FzQ' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/my-project-1516154946730/locations/global/registries/MAGNA_GPM_DEV_EDZKO/devices/GPM-1D.1/config?local_version=1'



########### MAGNA #####################################

python GetPWT.py -p pm-devices -k C:/Projects/GPS/UBlox/GPM_Monitor/GPM_Vibration/rsa_pm1_private.pem

set /P PVT=<JWT.tok

set OPENSSL_CONF=C:\Projects\GPS\UBlox\GPM_Monitor\openssl-1.1\ssl\openssl.cnf
openssl req -x509 -nodes -newkey rsa:2048 -keyout rsa_private.pem  -out rsa_cert.pem -subj "/CN=unused"


"C:\Program Files\Utils\bin\curl.exe" -X POST -H "authorization: Bearer %PVT%" -H "content-type: application/json" --data "{\"binary_data\": \"DATA\"}" -H "cache-control: no-cache" "https://cloudiotdevice.googleapis.com/v1/projects/pm-devices/locations/us-central1/registries/pm_telemetry/devices/PM-1:publishEvent"

"C:\Program Files\Utils\bin\curl.exe" -X POST -H "authorization: Bearer %PVT%" -H "content-type: application/json" --data "{\"binary_data\": \"DATA\", \"sub_folder\": \"pm_telemetry\"}" -H "cache-control: no-cache" "https://cloudiotdevice.googleapis.com/v1/projects/pm-devices/locations/us-central1/registries/pm_telemetry/devices/PM-1:publishEvent"

