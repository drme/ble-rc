using System;
namespace SmartRacerShared
{
	public interface ISettings
	{
		ushort BatteryCapacity1 { get; set; }
		ushort Capabilities1 { get; set; }
		string CarName1 { get; set; }
		LightsSettings LightsSettings1 { get; set; }
		ushort MaxBatteryVoltage1 { get; set; }
		ushort MaxCurrent1 { get; set; }
		int MaxSteeringValue { get; }
		ushort MinBatteryVoltage1 { get; set; }
		void Save();
		ChannelLimits SteeringSettings1 { get; set; }
		ChannelLimits ThrottleSettings1 { get; set; }
	}
}
