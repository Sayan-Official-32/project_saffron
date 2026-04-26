from supabase import create_client, Client
from app.core.config import settings

class DatabaseService:
    def __init__(self):
        self.client: Client = create_client(settings.SUPABASE_URL, settings.SUPABASE_KEY)

    def get_latest_sensors(self):
        response = self.client.table("sensor_data").select("*").order("created_at", desc=True).limit(1).execute()
        return response.data

    def get_sensor_history(self, limit: int = 100):
        response = self.client.table("sensor_data").select("*").order("created_at", desc=True).limit(limit).execute()
        return response.data

    def get_actuators(self):
        response = self.client.table("actuators").select("*").eq("id", 1).execute()
        return response.data

    def update_actuators(self, update_data: dict):
        response = self.client.table("actuators").update(update_data).eq("id", 1).execute()
        return response.data

db = DatabaseService()
