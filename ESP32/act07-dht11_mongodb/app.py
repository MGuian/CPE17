from datetime import datetime, timezone
from typing import List, Optional
from bson import ObjectId
from fastapi import FastAPI, HTTPException, Query, status
from motor.motor_asyncio import AsyncIOMotorClient
from pydantic import BaseModel, Field
import uvicorn

# Configuration
MONGO_URI = "mongodb://localhost:27017"
DATABASE_NAME = "sensor_db"
COLLECTION_NAME = "readings"

client: AsyncIOMotorClient = None


async def lifespan(app: FastAPI):
    global client
    client = AsyncIOMotorClient(MONGO_URI)
    print("Connected to MongoDB.")
    yield
    client.close()
    print("Closed MongoDB connection.")


app = FastAPI(
    title="Sensor Monitoring API",
    description="Single-file FastAPI CRUD for Temperature & Humidity readings",
    lifespan=lifespan,
)


def get_collection():
    return client[DATABASE_NAME][COLLECTION_NAME]


# Helper to convert MongoDB document to response dict
def format_doc(doc: dict) -> dict:
    return {
        "id": str(doc["_id"]),
        "sensor_id": doc["sensor_id"],
        "temperature": doc["temperature"],
        "humidity": doc["humidity"],
        "location": doc.get("location"),
        "timestamp": doc["timestamp"],
    }


# Pydantic Schemas
class SensorReadingCreate(BaseModel):
    sensor_id: str = Field(..., example="DHT22-001")
    temperature: float = Field(..., example=24.5)
    humidity: float = Field(..., ge=0.0, le=100.0, example=58.2)
    location: Optional[str] = Field(default="Room A", example="Greenhouse-1")


class SensorReadingUpdate(BaseModel):
    sensor_id: Optional[str] = None
    temperature: Optional[float] = None
    humidity: Optional[float] = Field(default=None, ge=0.0, le=100.0)
    location: Optional[str] = None


class SensorReadingResponse(SensorReadingCreate):
    id: str
    timestamp: datetime


# Endpoints
@app.post("/sensors/", response_model=SensorReadingResponse, status_code=status.HTTP_201_CREATED)
async def create_reading(reading: SensorReadingCreate):
    col = get_collection()
    doc = reading.model_dump()
    doc["timestamp"] = datetime.now(timezone.utc)
    result = await col.insert_one(doc)
    created = await col.find_one({"_id": result.inserted_id})
    return format_doc(created)


@app.get("/sensors/", response_model=List[SensorReadingResponse])
async def list_readings(
    skip: int = Query(0, ge=0),
    limit: int = Query(50, ge=1, le=500),
    sensor_id: Optional[str] = Query(None, description="Filter by sensor hardware ID"),
):
    col = get_collection()
    query = {"sensor_id": sensor_id} if sensor_id else {}
    cursor = col.find(query).sort("timestamp", -1).skip(skip).limit(limit)
    readings = await cursor.to_list(length=limit)
    return [format_doc(doc) for doc in readings]


@app.get("/sensors/{reading_id}", response_model=SensorReadingResponse)
async def get_reading(reading_id: str):
    if not ObjectId.is_valid(reading_id):
        raise HTTPException(status_code=400, detail="Invalid ObjectId format")
    col = get_collection()
    doc = await col.find_one({"_id": ObjectId(reading_id)})
    if not doc:
        raise HTTPException(status_code=404, detail="Reading not found")
    return format_doc(doc)


@app.put("/sensors/{reading_id}", response_model=SensorReadingResponse)
async def update_reading(reading_id: str, reading: SensorReadingUpdate):
    if not ObjectId.is_valid(reading_id):
        raise HTTPException(status_code=400, detail="Invalid ObjectId format")

    update_data = {k: v for k, v in reading.model_dump().items() if v is not None}
    if not update_data:
        raise HTTPException(status_code=400, detail="No fields provided to update")

    col = get_collection()
    result = await col.update_one({"_id": ObjectId(reading_id)}, {"$set": update_data})
    if result.matched_count == 0:
        raise HTTPException(status_code=404, detail="Reading not found")

    updated = await col.find_one({"_id": ObjectId(reading_id)})
    return format_doc(updated)


@app.delete("/sensors/{reading_id}", status_code=status.HTTP_204_NO_CONTENT)
async def delete_reading(reading_id: str):
    if not ObjectId.is_valid(reading_id):
        raise HTTPException(status_code=400, detail="Invalid ObjectId format")
    col = get_collection()
    result = await col.delete_one({"_id": ObjectId(reading_id)})
    if result.deleted_count == 0:
        raise HTTPException(status_code=404, detail="Reading not found")
    return None


if __name__ == "__main__":
    uvicorn.run("app:app", host="0.0.0.0", port=8000, reload=True)