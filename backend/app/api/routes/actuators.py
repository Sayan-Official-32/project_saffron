from fastapi import APIRouter
from pydantic import BaseModel
from typing import Optional
from app.services.db import db

router = APIRouter()

class ActuatorUpdate(BaseModel):
    mist_maker: Optional[bool] = None
    cooling_fan: Optional[bool] = None
    grow_light_pwm: Optional[int] = None
    auto_mode: Optional[bool] = None
    relay3: Optional[bool] = None
    relay4: Optional[bool] = None

@router.get("/actuators")
def get_actuators():
    """
    Fetches the actuator values from the Supabase database.
    """
    try:
        data = db.get_actuators()
        if data and len(data) > 0:
            return data[0]
        return {"error": "No actuators logic found"}
    except Exception as e:
        return {"error": str(e)}

@router.post("/actuators")
def update_actuators(data: ActuatorUpdate):
    """
    Updates the actuator values in the Supabase database.
    """
    try:
        update_data = {k: v for k, v in data.dict().items() if v is not None}
        if not update_data:
            return {"error": "No fields to update"}
            
        result = db.update_actuators(update_data)
        
        if result:
            return {"status": "success", "data": result[0]}
        return {"error": "Update failed (maybe no row with id=1?)"}
    except Exception as e:
        return {"error": str(e)}
