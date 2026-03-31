// Fill out your copyright notice in the Description page of Project Settings.


#include "PythonCommunication/SignalTG.h"


FSignalTG::FSignalTG():
	Type{ SignalValueType::None },
	Value{ std::monostate{} },
	PayloadLayout{ 0 }
{
	TRACE("Default constructor")
}

FSignalTG::FSignalTG(const FSignalTG& other) :
	Type(other.Type),
	Value(other.Value),
	PayloadLayout(other.PayloadLayout)
{
	TRACE("Copy constructor")
}

FSignalTG::FSignalTG(FSignalTG&& other) :
	Type(std::move(other.Type)),
	Value(std::move(other.Value)),
	PayloadLayout(std::move(other.PayloadLayout))
{
	TRACE("Move constructor")
}

FSignalTG& FSignalTG::operator=(const FSignalTG& other)
{
	TRACE("Copy operator=")
	this->Type = other.Type;
	this->Value = other.Value;
	this->PayloadLayout = other.PayloadLayout;
	return *this;
}

FSignalTG& FSignalTG::operator=(FSignalTG&& other)
{
	TRACE("Move operator=")
	this->Type = std::move(other.Type);
	this->Value = std::move(other.Value);
	this->PayloadLayout = std::move(other.PayloadLayout);
	return *this;
}

bool FSignalTG::Serialize(Payload& OutBuffer)
{
	TRACE("Type: %d", Type)
	OutBuffer.Append(ToBytes(Type), sizeof(uint8));
	TRACEBYTES(OutBuffer.GetData(), OutBuffer.Num())

	TRACE("PayloadLayout: %d", PayloadLayout)
	OutBuffer.Append(ToBytes(PayloadLayout), sizeof(uint8));
	TRACEBYTES(OutBuffer.GetData(), OutBuffer.Num())

	const auto typeNum = static_cast<uint8>(Type);
	if ((Value.index() != typeNum) || (Value.index() == 0))
	{
		TRACEERROR("Wrong type! %lld / %d", Value.index(), typeNum)
		return false;
	}
	else
	{
		if (std::holds_alternative<std::monostate>(Value))
		{
			TRACEERROR("Wrong value - monostate!")
			return false;
		}
		else if (std::holds_alternative<Payload>(Value))
		{
			TRACE("Value (Payload): ")
			Payload value = std::get<Payload>(Value);
			TRACEBYTES(value.GetData(), value.Num())
			OutBuffer.Append(value.GetData(), value.Num());
		}
		else
		{
			std::visit([this, &OutBuffer](auto v)
				{
					TRACE("Holds Numeric Value")
					OutBuffer.Append(ToBytes(v), (SignalValueTypeSizes[Type]));
				}, Value);
		}
	}
	TRACEBYTES(OutBuffer.GetData(), OutBuffer.Num())
	return true;
}

size_t FSignalTG::Deserialize(Payload& data, size_t startPos)
{
	TRACE("Deserializing %d bytes of data starting at position: %d", 
		data.Num(), startPos);
	if ((startPos + 3) > data.Num())
	{
		TRACEERROR("Read out of range of given data!");
		return startPos;
	}
	size_t currBufPos = startPos;

	FMemory::Memcpy(&Type, data.GetData() + currBufPos, sizeof(SignalValueType));
	currBufPos += sizeof(SignalValueType);
	TRACE("Type: %d Deserializing at position: %d", Type, currBufPos);

	FMemory::Memcpy(&PayloadLayout, data.GetData() + currBufPos, sizeof(uint8));
	currBufPos += sizeof(uint8);
	TRACE("PayloadLayout: %d Deserializing at position: %d", 
		PayloadLayout, currBufPos);

	if (SignalValueType::None == Type)
	{
		TRACEERROR("Type is not set!");
		return currBufPos;
	}

	switch (Type)
	{
	case SignalValueType::UINT8:
	{
		uint8 temp = 0;
		FMemory::Memcpy(&temp, data.GetData() + currBufPos, sizeof(uint8));
		SetValue<uint8>(temp);
		uint8* val = GetValue<uint8>();
		if (val)
		{
			currBufPos += SignalValueTypeSizes[SignalValueType::UINT8];
			TRACE("UINT8 Value: %d currBufPos position: %d", *val, currBufPos)
		}
		else
			TRACEERROR("UINT8 Value is empty!")
	}
		break;
	case SignalValueType::UINT16:
	{
		uint16 temp = 0;
		FMemory::Memcpy(&temp, data.GetData() + currBufPos, sizeof(uint16));
		SetValue<uint8>(temp);
		uint16* val = GetValue<uint16>();
		if (val)
		{
			currBufPos += SignalValueTypeSizes[SignalValueType::UINT16];
			TRACE("uint16 Value: %d currBufPos position: %d", *val, currBufPos)
		}
		else
			TRACEERROR("uint16 Value is empty!")
	}
		break;
	case SignalValueType::UINT32:
	{
		uint32 temp = 0;
		FMemory::Memcpy(&temp, data.GetData() + currBufPos, sizeof(uint32));
		SetValue<uint32>(temp);
		uint32* val = GetValue<uint32>();
		if (val)
		{
			currBufPos += SignalValueTypeSizes[SignalValueType::UINT32];
			TRACE("uint32 Value: %d currBufPos position: %d", *val, currBufPos)
		}
		else
			TRACEERROR("uint32 Value is empty!")
	}
		break;
	case SignalValueType::UINT64:
	{
		uint64 temp = 0;
		FMemory::Memcpy(&temp, data.GetData() + currBufPos, sizeof(uint64));
		SetValue<uint64>(temp);
		uint64* val = GetValue<uint64>();
		if (val)
		{
			currBufPos += SignalValueTypeSizes[SignalValueType::UINT64];
			TRACE("uint64 Value: %d currBufPos position: %d", *val, currBufPos)
		}
		else
			TRACEERROR("uint64 Value is empty!")
	}
		break;
	case SignalValueType::FLOAT:
	{
		float temp = 0.f;
		FMemory::Memcpy(&temp, data.GetData() + currBufPos, sizeof(float));
		SetValue<float>(temp);
		float* val = GetValue<float>();
		if (val)
		{
			currBufPos += SignalValueTypeSizes[SignalValueType::FLOAT];
			TRACE("float Value: %f currBufPos position: %d", *val, currBufPos)
		}
		else
			TRACEERROR("float Value is empty!")
	}
		break;
	case SignalValueType::DOUBLE:
	{
		double temp = 0.0;
		FMemory::Memcpy(&temp, data.GetData() + currBufPos, sizeof(double));
		SetValue<double>(temp);
		double* val = GetValue<double>();
		if (val)
		{
			currBufPos += SignalValueTypeSizes[SignalValueType::DOUBLE];
			TRACE("double Value: %lf currBufPos position: %d", *val, currBufPos)
		}
		else
			TRACEERROR("double Value is empty!")
	}
		break;
	case SignalValueType::PAYLOAD:
	{
		Payload temp = Payload();
		temp.Append(&data[currBufPos], data.Num() - currBufPos);
		SetValue<Payload>(std::move(temp));
		Payload* val = GetValue<Payload>();
		if (val) 
		{
			currBufPos = data.Num();
			TRACE("currBufPos position: %d Payload Value as follows", currBufPos)
			TRACEBYTES(val->GetData(), val->Num())
		}
		else
			TRACEERROR("Payload Value is empty!")
	}
		break;
	default:
		TRACEWARN("Unrecognized type: %d", static_cast<uint8>(Type));
	}
	return currBufPos;
}
