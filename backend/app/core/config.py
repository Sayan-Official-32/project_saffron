import os
from dotenv import load_dotenv

# Load environment variables from .env file
load_dotenv()

class Settings:
    SUPABASE_URL = os.getenv("SUPABASE_URL", "https://jhgnrbujsggsllzkgsfb.supabase.co")
    SUPABASE_KEY = os.getenv("SUPABASE_KEY", "sb_publishable_RnCtzZEfz-BYwDZ7_X73iw_Q_hdk44_")
    
    # Project Paths
    BASE_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    FRONTEND_DIR = os.path.join(BASE_DIR, "..", "frontend")
    ML_MODEL_DIR = os.path.join(BASE_DIR, "..", "ml_model", "model")
    DATASETS_DIR = os.path.join(BASE_DIR, "..", "ml_model", "datasets")

settings = Settings()
