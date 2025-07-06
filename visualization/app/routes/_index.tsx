import type { MetaFunction } from "@remix-run/node";

export const meta: MetaFunction = () => {
  return [
    { title: "unoRadar" },
    { name: "description", content: "Arduino Radar Visualization" },
  ];
};

export default function Index() {
  return (
    <div className="min-h-screen bg-black">
      {/* Empty black page */}
    </div>
  );
}