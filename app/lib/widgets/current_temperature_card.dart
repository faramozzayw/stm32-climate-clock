import 'package:flutter/material.dart';

import '../utils/temperature.dart';

class CurrentTemperatureCard extends StatelessWidget {
  const CurrentTemperatureCard({
    required this.temperatureTenths,
    required this.fahrenheit,
    super.key,
  });

  final int? temperatureTenths;
  final bool fahrenheit;

  @override
  Widget build(BuildContext context) {
    final temperature = temperatureTenths;

    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 20, vertical: 18),
      decoration: BoxDecoration(
        color: Colors.white.withValues(alpha: 0.92),
        borderRadius: BorderRadius.circular(24),
        border: Border.all(color: Colors.white),
        boxShadow: [
          BoxShadow(
            color: const Color(0xFF40375E).withValues(alpha: 0.08),
            blurRadius: 24,
            offset: const Offset(0, 10),
          ),
        ],
      ),
      child: Row(
        children: [
          Container(
            width: 52,
            height: 52,
            decoration: BoxDecoration(
              color: const Color(0xFFFFF0E8),
              borderRadius: BorderRadius.circular(18),
            ),
            child: const Icon(
              Icons.device_thermostat_rounded,
              color: Color(0xFFF28A58),
              size: 30,
            ),
          ),
          const SizedBox(width: 15),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Current temperature',
                  style: Theme.of(context).textTheme.titleMedium,
                ),
                const SizedBox(height: 3),
                Text(
                  temperature == null
                      ? 'Waiting for device data'
                      : 'Live from device',
                  style: Theme.of(context).textTheme.bodySmall?.copyWith(
                    color: const Color(0xFF777287),
                  ),
                ),
              ],
            ),
          ),
          const SizedBox(width: 12),
          Text(
            temperature == null
                ? '--.- °${fahrenheit ? 'F' : 'C'}'
                : formatTemperatureWithUnit(
                    temperature,
                    fahrenheit: fahrenheit,
                  ),
            style: const TextStyle(
              color: Color(0xFF29263A),
              fontSize: 25,
              fontWeight: FontWeight.w800,
              letterSpacing: -0.7,
            ),
          ),
        ],
      ),
    );
  }

}
