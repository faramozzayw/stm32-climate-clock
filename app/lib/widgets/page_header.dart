import 'package:flutter/material.dart';

class PageHeader extends StatelessWidget {
  const PageHeader({
    required this.fahrenheit,
    required this.onTemperatureUnitChanged,
    super.key,
  });

  final bool fahrenheit;
  final ValueChanged<bool> onTemperatureUnitChanged;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Container(
          width: 52,
          height: 52,
          decoration: BoxDecoration(
            color: const Color(0xFF6D5DD3),
            borderRadius: BorderRadius.circular(18),
            boxShadow: [
              BoxShadow(
                color: const Color(0xFF6D5DD3).withValues(alpha: 0.24),
                blurRadius: 18,
                offset: const Offset(0, 8),
              ),
            ],
          ),
          child: const Icon(
            Icons.thermostat_rounded,
            color: Colors.white,
            size: 30,
          ),
        ),
        const SizedBox(width: 14),
        Expanded(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Text(
                'ClimateClock',
                style: Theme.of(context).textTheme.headlineMedium,
              ),
              const SizedBox(height: 2),
              Text(
                'Live temperature, thresholds, and alerts',
                style: Theme.of(context).textTheme.bodyMedium?.copyWith(
                  color: const Color(0xFF777287),
                ),
              ),
            ],
          ),
        ),
        const SizedBox(width: 8),
        _TemperatureUnitToggle(
          fahrenheit: fahrenheit,
          onChanged: onTemperatureUnitChanged,
        ),
      ],
    );
  }
}

class _TemperatureUnitToggle extends StatelessWidget {
  const _TemperatureUnitToggle({
    required this.fahrenheit,
    required this.onChanged,
  });

  final bool fahrenheit;
  final ValueChanged<bool> onChanged;

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 94,
      height: 42,
      padding: const EdgeInsets.all(4),
      decoration: BoxDecoration(
        color: Colors.white.withValues(alpha: 0.72),
        borderRadius: BorderRadius.circular(16),
        border: Border.all(color: const Color(0xFFE4DFF1)),
        boxShadow: [
          BoxShadow(
            color: const Color(0xFF413874).withValues(alpha: 0.08),
            blurRadius: 14,
            offset: const Offset(0, 6),
          ),
        ],
      ),
      child: Stack(
        children: [
          AnimatedAlign(
            duration: const Duration(milliseconds: 220),
            curve: Curves.easeOutCubic,
            alignment: fahrenheit
                ? Alignment.centerRight
                : Alignment.centerLeft,
            child: Container(
              width: 42,
              height: 34,
              decoration: BoxDecoration(
                color: const Color(0xFF6D5DD3),
                borderRadius: BorderRadius.circular(12),
                boxShadow: [
                  BoxShadow(
                    color: const Color(0xFF6D5DD3).withValues(alpha: 0.28),
                    blurRadius: 10,
                    offset: const Offset(0, 4),
                  ),
                ],
              ),
            ),
          ),
          Row(
            children: [
              _TemperatureUnitOption(
                label: '°C',
                selected: !fahrenheit,
                onTap: () => onChanged(false),
              ),
              _TemperatureUnitOption(
                label: '°F',
                selected: fahrenheit,
                onTap: () => onChanged(true),
              ),
            ],
          ),
        ],
      ),
    );
  }
}

class _TemperatureUnitOption extends StatelessWidget {
  const _TemperatureUnitOption({
    required this.label,
    required this.selected,
    required this.onTap,
  });

  final String label;
  final bool selected;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return Expanded(
      child: Semantics(
        button: true,
        selected: selected,
        label: '$label temperature unit',
        child: InkWell(
          onTap: onTap,
          borderRadius: BorderRadius.circular(12),
          child: Center(
            child: AnimatedDefaultTextStyle(
              duration: const Duration(milliseconds: 180),
              style: TextStyle(
                color: selected ? Colors.white : const Color(0xFF777287),
                fontSize: 14,
                fontWeight: FontWeight.w800,
              ),
              child: Text(label),
            ),
          ),
        ),
      ),
    );
  }
}
