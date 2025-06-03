from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from fastapi.middleware.cors import CORSMiddleware
import json
import os

app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"]
)

@app.get("/")
def hello():
    return {"message": "Hello from FastAPI on Windows!"}

SAVE_PATH = "navigation_data.json"

@app.post("/set-navigation")
async def set_navigation(request: Request):
    data = await request.json()

    with open(SAVE_PATH, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=2)

    return {"status": "saved"}

@app.get("/get-navigation")
def get_navigation():
    if not os.path.exists(SAVE_PATH):
        return JSONResponse(content={"error": "No route data yet"}, status_code=404)

    with open(SAVE_PATH, "r", encoding="utf-8") as f:
        geojson = json.load(f)
    return geojson