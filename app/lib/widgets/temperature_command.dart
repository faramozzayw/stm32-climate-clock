import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../utils/temperature.dart';

class TemperatureCommand extends StatelessWidget {
  const TemperatureCommand({
    super.key,
    required this.label,
    required this.hint,
    required this.icon,
    required this.accent,
    required this.surface,
    required this.controller,
    required this.currentLimitTenths,
    required this.fahrenheit,
    required this.onSend,
  });

  final String label;
  final String hint;
  final IconData icon;
  final Color accent;
  final Color surface;
  final TextEditingController controller;
  final int? currentLimitTenths;
  final bool fahrenheit;
  final VoidCallback? onSend;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(18),
      decoration: BoxDecoration(
        color: surface,
        borderRadius: BorderRadius.circular(24),
        border: Border.all(color: accent.withValues(alpha: 0.12)),
      ),
      child: Column(
        children: [
          Row(
            children: [
              Container(
                width: 42,
                height: 42,
                decoration: BoxDecoration(
                  color: accent.withValues(alpha: 0.14),
                  borderRadius: BorderRadius.circular(14),
                ),
                child: Icon(icon, color: accent),
              ),
              const SizedBox(width: 12),
              Expanded(
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    Text(label, style: Theme.of(context).textTheme.titleMedium),
                    Text(
                      hint,
                      style: Theme.of(context).textTheme.bodySmall?.copyWith(
                        color: const Color(0xFF777287),
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          Align(
            alignment: Alignment.centerLeft,
            child: Text(
              currentLimitTenths == null
                  ? 'Current device limit: --.- °${fahrenheit ? 'F' : 'C'}'
                  : 'Current device limit: '
                        '${formatTemperatureWithUnit(currentLimitTenths!, fahrenheit: fahrenheit)}',
              style: Theme.of(context).textTheme.bodySmall?.copyWith(
                color: accent,
                fontWeight: FontWeight.w700,
              ),
            ),
          ),
          const SizedBox(height: 10),
          Row(
            children: [
              Expanded(
                child: TextField(
                  controller: controller,
                  keyboardType: const TextInputType.numberWithOptions(
                    signed: true,
                    decimal: true,
                  ),
                  inputFormatters: [
                    FilteringTextInputFormatter.allow(RegExp(r'^-?\d*\.?\d?')),
                  ],
                  style: const TextStyle(
                    fontSize: 22,
                    fontWeight: FontWeight.w700,
                    color: Color(0xFF29263A),
                  ),
                  decoration: InputDecoration(
                    filled: true,
                    fillColor: Colors.white.withValues(alpha: 0.8),
                    suffixText: '°${fahrenheit ? 'F' : 'C'}',
                    suffixStyle: TextStyle(
                      color: accent,
                      fontWeight: FontWeight.w700,
                    ),
                    contentPadding: const EdgeInsets.symmetric(
                      horizontal: 16,
                      vertical: 12,
                    ),
                    border: OutlineInputBorder(
                      borderRadius: BorderRadius.circular(16),
                      borderSide: BorderSide.none,
                    ),
                  ),
                ),
              ),
              const SizedBox(width: 12),
              IconButton.filled(
                onPressed: onSend,
                style: IconButton.styleFrom(
                  backgroundColor: accent,
                  foregroundColor: Colors.white,
                  disabledBackgroundColor: accent.withValues(alpha: 0.2),
                  minimumSize: const Size(54, 54),
                ),
                tooltip: 'Send $label',
                icon: const Icon(Icons.arrow_upward_rounded),
              ),
            ],
          ),
        ],
      ),
    );
  }

}
