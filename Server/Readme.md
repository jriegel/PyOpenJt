# Geomtry server

This Python REST server manages converting and caching of OpenJT to glTf. 

It consist of two HTTP endpoints. The first is the service which managing the translation from Jt to glTF and caching the result. It delivers a Url to the caller where the glTf file can be downloaded.

There fore it write the cache to a HTTP-root folder which gets delivered by one or more normal web server, e.g. Apache-HTTPD.

## Prerequisite  

    pip install fastapi uvicorn