REM
python GetPWT.py -p my-project-1516154946730 -k C:/users/edzko/rsa_private.pem

curl -X POST -H 'authorization: Bearer JWT' -H 'content-type: application/json' --data '{"binary_data": "DATA"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/{project-id}/locations/{cloud-region}/registries/{registry-id}/devices/{device-id}:publishEvent'


curl -X POST -H 'authorization: eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE2MjQ5MDA2OTYsImV4cCI6MTYyNDkwNDI5NiwiYXVkIjoibXktcHJvamVjdC0xNTE2MTU0OTQ2NzMwIn0.LFfTCWKBc9bIvMdTsq3u7jNUKnu_QKcKZvO6lJQ8mvcqURaB6vZYBV_5gdI8rJ7xbS5SUdo9vp26refwBNl9HY9eg0ASsjKxcuqZUzNisbLfw_dubol2ejzP3U0CRoMkAcQhomfue7FrZ4dmBFIgnXOmx9Hj5DX9tfxHqVjtquexdjCU1o2IJarLfVp17kgiqQCzyCTl_KqAgnopf0fimhCU1m7hAGGN6ou08YPrY3IOHVYaHaJ2rajSPQxjecLDEW7wLG3jQbD3FuFLHQZ4t9881H1_cqdKV_CNOWryFP4qtK8YNgTOLscCUkmh8_JhtBdy3SaaElr5gaiNSU0FzQ' -H 'content-type: application/json' --data '{"binary_data": "my first data", "VIB_DATA": "VIB_DATA"}' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/my-project-1516154946730/locations/global/registries/MAGNA_GPM_DEV_EDZKO/devices/GPM-1D.1:publishEvent' --libcurl iot.c


curl -H 'authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJpYXQiOjE2MjQ5MDA2OTYsImV4cCI6MTYyNDkwNDI5NiwiYXVkIjoibXktcHJvamVjdC0xNTE2MTU0OTQ2NzMwIn0.LFfTCWKBc9bIvMdTsq3u7jNUKnu_QKcKZvO6lJQ8mvcqURaB6vZYBV_5gdI8rJ7xbS5SUdo9vp26refwBNl9HY9eg0ASsjKxcuqZUzNisbLfw_dubol2ejzP3U0CRoMkAcQhomfue7FrZ4dmBFIgnXOmx9Hj5DX9tfxHqVjtquexdjCU1o2IJarLfVp17kgiqQCzyCTl_KqAgnopf0fimhCU1m7hAGGN6ou08YPrY3IOHVYaHaJ2rajSPQxjecLDEW7wLG3jQbD3FuFLHQZ4t9881H1_cqdKV_CNOWryFP4qtK8YNgTOLscCUkmh8_JhtBdy3SaaElr5gaiNSU0FzQ' -H 'cache-control: no-cache' 'https://cloudiotdevice.googleapis.com/v1/projects/my-project-1516154946730/locations/global/registries/MAGNA_GPM_DEV_EDZKO/devices/GPM-1D.1/config?local_version=1'



########### MAGNA #####################################

REM create JWT and store in JWT.tok
python GetJWT.py -p pm-devices -k C:/Projects/GPS/UBlox/GPM_Monitor/GPM_Vibration/rsa_pm1_private.pem

set /P PVT=<JWT.tok

set OPENSSL_CONF=C:\Projects\GPS\UBlox\GPM_Monitor\openssl-1.1\ssl\openssl.cnf
openssl req -x509 -nodes -newkey rsa:2048 -keyout rsa_private.pem  -out rsa_cert.pem -subj "/CN=unused"

"C:\Program Files\Utils\bin\curl.exe" -H "authorization: Bearer %PVT%" -H "cache-control: no-cache" "https://cloudiotdevice.googleapis.com/v1/projects/pm-devices/locations/us-central1/registries/pm_telemetry/devices/PM-1/config?local_version=1"

"C:\Program Files\Utils\bin\curl.exe" -X POST -H "authorization: Bearer %PVT%" -H "content-type: application/json" --data "{\"binary_data\": \"DATA\"}" -H "cache-control: no-cache" "https://cloudiotdevice.googleapis.com/v1/projects/pm-devices/locations/us-central1/registries/pm_telemetry/devices/PM-1:publishEvent"

"C:\Program Files\Utils\bin\curl.exe" -X POST -H "authorization: Bearer %PVT%" -H "content-type: application/json" --data "{\"binary_data\": \"DATA\", \"sub_folder\": \"pm_telemetry\"}" -H "cache-control: no-cache" "https://cloudiotdevice.googleapis.com/v1/projects/pm-devices/locations/us-central1/registries/pm_telemetry/devices/PM-1:publishEvent"

"C:\Program Files\Utils\bin\curl.exe" -H "authorization: Bearer %PVT%" -H "cache-control: no-cache" "https://cloudiotdevice.googleapis.com/v1/projects/pm-devices/locations/us-central1/registries/pm_telemetry/devices/PM-1/config?local_version=0"


gcloud iot registries list --region us-central1 --project pm-devices

gcloud auth login curl-publisher@pm-devices.iam.gserviceaccount.com 

REM create token. Stores token in JWT.tok
REM getjwt.bat pm-devices rsa_pm1_private.pem


REM To publish to GOOGLE IOT

REM create the message payload
echo 'This is a message from Edzko' | openssl base64 > msgdata.txt
set /p MESSAGE=<msgdata.txt

REM create the message
echo {'messages': [{'data': '%MESSAGE%'}]} > msg.txt 

REM activate service account, refresh access token, and store in ACCESS_TOKEN every 1 hr
call gcloud auth activate-service-account curl-publisher@pm-devices.iam.gserviceaccount.com --key-file="pm-devices-20d93a2a3f55.json"
call gcloud auth print-access-token > token.asc
set /p ACCESS_TOKEN=<token.asc

set PROJECT=pm-devices
set TOPIC=pm_telemetry

curl -H "content-type: application/json" -H "Authorization: Bearer %ACCESS_TOKEN%" -X POST --data @msg.txt https://pubsub.googleapis.com/v1/projects/%PROJECT%/topics/%TOPIC%:publish

