from fastapi import APIRouter
from app.services.db import db

router = APIRouter()

@router.get("/sensors")
def get_latest_sensors():
    """
    Fetches the latest sensor data from the Supabase database.
    """
    try:
        data = db.get_latest_sensors()
        if data and len(data) > 0:
            return data[0]
        return {"error": "No data found"}
    except Exception as e:
        return {"error": str(e)}

@router.get("/sensor_history")
def get_sensor_history():
    """
    Fetches the raw sensor history for data analysis.
    """
    import datetime
    try:
        data = db.get_sensor_history(limit=100)
        if not data:
            return []
        
        data_records = list(reversed(data))
        results = []
        for record in data_records:
            created_at = record.get("created_at", "")
            date_str = ""
            if created_at:
                try:
                    dt = datetime.datetime.fromisoformat(created_at.replace("Z", "+00:00"))
                    date_str = dt.strftime("%H:%M:%S")
                except:
                    date_str = created_at[:10]
            
            results.append({
                "time": date_str,
                "temperature": float(record.get("temperature", 0)),
                "humidity": float(record.get("humidity", 0)),
                "moisture": float(record.get("moisture", 0)),
                "light": float(record.get("light", 0)),
                "co2": float(record.get("co2", 0))
            })
        return results
    except Exception as e:
        return {"error": str(e)}
