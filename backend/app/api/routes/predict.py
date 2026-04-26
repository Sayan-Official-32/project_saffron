from fastapi import APIRouter
from app.services.db import db
from app.services.ml import ml

router = APIRouter()

@router.get("/predict")
def predict_growth(phase: str = "phase1"):
    """
    Fetches the latest sensor data and predicts the growth rate
    based on the selected phase model.
    """
    try:
        # Get latest sensor data
        data = db.get_latest_sensors()
        if not data or len(data) == 0:
            return {"error": "No sensor data available for prediction"}

        latest_record = data[0]
        
        # Extract features
        features = {
            "Temperature": float(latest_record.get("temperature", 0)),
            "Humidity": float(latest_record.get("humidity", 0)),
            "Light": float(latest_record.get("light", 0)),
            "Soil_Moisture": float(latest_record.get("moisture", 0)),
            "CO2": float(latest_record.get("co2", 0))
        }

        # Predict
        prediction = ml.predict_growth(phase, features)
        
        return {
            "phase": phase,
            "predicted_growth": prediction
        }

    except Exception as e:
        return {"error": str(e)}
