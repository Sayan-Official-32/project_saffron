from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from supabase import create_client, Client

app = FastAPI()

# Allow CORS for your frontend to communicate with this backend
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"], # Allow all origins for local development
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Supabase Configuration
URL = "https://jhgnrbujsggsllzkgsfb.supabase.co"
KEY = "sb_publishable_RnCtzZEfz-BYwDZ7_X73iw_Q_hdk44_"
supabase: Client = create_client(URL, KEY)

@app.get("/api/sensors")
def get_latest_sensors():
    """
    Fetches the latest sensor data from the Supabase database.
    """
    try:
        response = supabase.table("sensor_data").select("*").order("created_at", desc=True).limit(1).execute()
        if response.data and len(response.data) > 0:
            return response.data[0]
        return {"error": "No data found"}
    except Exception as e:
        return {"error": str(e)}

@app.get("/api/actuators")
def get_actuators():
    """
    Fetches the actuator values from the Supabase database.
    """
    try:
        response = supabase.table("actuators").select("*").eq("id", 1).execute()
        if response.data and len(response.data) > 0:
            return response.data[0]
        return {"error": "No actuators logic found"}
    except Exception as e:
        return {"error": str(e)}
